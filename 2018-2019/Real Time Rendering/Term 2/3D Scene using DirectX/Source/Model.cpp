
#include "stdafx.h"
#include <Model.h>
#include <Material.h>
#include <Effect.h>
#include <iostream>
#include <exception>

#include <CoreStructures\CoreStructures.h>
#include <CGImport3\CGModel\CGModel.h>
#include <CGImport3\Importers\CGImporters.h>



using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace CoreStructures;


void Model::load(ID3D11Device *device, Effect *_effect, const std::wstring& filename,  Material *_material) {


	if (_material == NULL)
		material = new Material();
	else
		material = _material;
	material->setEmissive(XMCOLOR(1, 1, 1, 1));

	try
	{
		if (!device || !inputLayout)
			throw exception("Invalid parameters for Model instantiation");

		HRESULT hr=loadModelAssimp(device, filename);


		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexExt::createInputLayout(device, vsBytecode, &inputLayout);

		if (!SUCCEEDED(hr))
			throw exception("Cannot create input layout interface");
	
		D3D11_SAMPLER_DESC linearDesc;

		ZeroMemory(&linearDesc, sizeof(D3D11_SAMPLER_DESC));

		linearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		linearDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.MinLOD = 0.0f;
		linearDesc.MaxLOD = 0.0f;
		linearDesc.MipLODBias = 0.0f;
		//linearDesc.MaxAnisotropy = 0; // Unused for isotropic filtering
		linearDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&linearDesc, &sampler);
	}
	catch (exception& e)
	{
		cout << "Model could not be instantiated due to:\n";
		cout << e.what() << endl;


		if (vertexBuffer)
			vertexBuffer->Release();

		if (indexBuffer)
			indexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		inputLayout = nullptr;

		numMeshes = 0;
	}
}

Model::~Model() {

}

//void Model::update(ID3D11DeviceContext *context) {

void Model::render(ID3D11DeviceContext *context) {//, int mode

	effect->bindPipeline(context);

	context->PSSetConstantBuffers(0, 1, &cBufferModelGPU);
	context->VSSetConstantBuffers(0, 1, &cBufferModelGPU);

	// Validate Model before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !indexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set Model vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(ExtendedVertexStruct) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (numTextures>0 && sampler) {

		context->PSSetShaderResources(0, numTextures, textures);
		context->PSSetSamplers(0, 1, &sampler);
	}


	// Set vertex layout
	context->IASetInputLayout(inputLayout);



	// Apply the cBuffer.
	context->VSSetConstantBuffers(0, 1, &cBufferModelGPU);
	context->PSSetConstantBuffers(0, 1, &cBufferModelGPU);

	// Draw Model
	for (uint32_t indexOffset = 0, i = 0; i < numMeshes; indexOffset += indexCount[i], ++i)
		context->DrawIndexed(indexCount[i], indexOffset, baseVertexOffset[i]);	
	

}


HRESULT Model::loadModel(ID3D11Device *device, const std::wstring& filename)
{
	CGModel *actualModel = nullptr;
	ExtendedVertexStruct *_vertexBuffer = nullptr;
	uint32_t *_indexBuffer = nullptr;

	try
	{
	actualModel = new CGModel();




	if (!actualModel)
		throw exception("Cannot create model to import");


	// Get filename extension
	wstring ext = filename.substr(filename.length() - 4);

	CG_IMPORT_RESULT cg_err;

	if (0 == ext.compare(L".gsf"))
		cg_err = importGSF(filename.c_str(), actualModel);
	else if (0 == ext.compare(L".3ds"))
		cg_err = import3DS(filename.c_str(), actualModel);
	else if (0 == ext.compare(L".obj"))
		cg_err = importOBJ(filename.c_str(), actualModel);
	else
		throw exception("Object file format not supported");

	if (cg_err != CG_IMPORT_OK)
		throw exception("Could not load model");


	// Build a buffer for each mesh

	// Generate a single buffer object that stores a CGVertexExt struct per vertex
	// Each CGPolyMesh in actualModel is stored contiguously in the buffer.
	// The indices are also stored in the same way but no offset to each vertex sub-buffer is added.
	// Model stores a vector of base vertex offsets to point to the start of each sub-buffer and start index offsets for each sub-mesh

	numMeshes = actualModel->getMeshCount();

	if (numMeshes == 0)
		throw exception("Empty model loaded");

	uint32_t numVertices = 0;
	uint32_t numIndices = 0;

	for (uint32_t i = 0; i < numMeshes; ++i) {

		// Store base vertex index;
		baseVertexOffset.push_back(numVertices);

		CGPolyMesh *M = actualModel->getMeshAtIndex(i);

		if (M) {

			// Increment vertex count
			numVertices += M->vertexCount();

			// Store num indices for current mesh
			indexCount.push_back(M->faceCount() * 3);
			numIndices += M->faceCount() * 3;
		}
	}


	// Create vertex buffer
	_vertexBuffer = (ExtendedVertexStruct*)malloc(numVertices * sizeof(ExtendedVertexStruct));

	if (!_vertexBuffer)
		throw exception("Cannot create vertex buffer");


	// Create index buffer
	_indexBuffer = (uint32_t*)malloc(numIndices * sizeof(uint32_t));

	if (!_indexBuffer)
		throw exception("Cannot create index buffer");


	// Copy vertex data into single buffer
	ExtendedVertexStruct *vptr = _vertexBuffer;
	uint32_t *indexPtr = _indexBuffer;

	for (uint32_t i = 0; i < numMeshes; ++i) {

		// Get mesh data (assumes 1:1 correspondance between vertex position, normal and texture coordinate data)
		CGPolyMesh *M = actualModel->getMeshAtIndex(i);

		if (M) {

			CGBaseMeshDefStruct R;
			M->createMeshDef(&R);

			for (uint32_t k = 0; k < uint32_t(R.N); ++k, vptr++) {

				vptr->pos = XMFLOAT3(R.V[k].x, R.V[k].y, R.V[k].z);
				vptr->normal = XMFLOAT3(R.Vn[k].x, R.Vn[k].y, R.Vn[k].z);

				//Flip normal.x for OBJ & GSF (might be required for other files too?)
				if (0 == ext.compare(L".obj") || 0 == ext.compare(L".gsf"))// || 0 == ext.compare(L".3ds")
				{
					vptr->normal.x = -vptr->normal.x;
					vptr->pos.x = -vptr->pos.x;
				}
				if (R.Vt && R.VtSize > 0)
					vptr->texCoord = XMFLOAT2(R.Vt[k].s, 1.0f - R.Vt[k].t);
				else
					vptr->texCoord = XMFLOAT2(0.0f, 0.0f);

				vptr->matDiffuse = material->getColour()->diffuse;//XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				vptr->matSpecular = material->getColour()->specular;// XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// Copy mesh indices from CGPolyMesh into buffer
			memcpy(indexPtr, R.Fv, R.n * sizeof(CGFaceVertex));

			// Re-order indices to account for DirectX using the left-handed coordinate system
			for (int k = 0; k < R.n; ++k, indexPtr += 3)
				swap(indexPtr[0], indexPtr[2]);
		}
	}


	//
	// Setup DX vertex buffer interfaces
	//

	D3D11_BUFFER_DESC vertexDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = numVertices * sizeof(ExtendedVertexStruct);
	vertexData.pSysMem = _vertexBuffer;

	HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

	if (!SUCCEEDED(hr))
		throw exception("Vertex buffer cannot be created");


	// Setup index buffer
	D3D11_BUFFER_DESC indexDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));

	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = numIndices * sizeof(uint32_t);
	indexData.pSysMem = _indexBuffer;

	hr = device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);

	if (!SUCCEEDED(hr))
		throw exception("Index buffer cannot be created");


	// Dispose of local resources
	free(_vertexBuffer);
	free(_indexBuffer);
	actualModel->release();
	}
	catch (exception& e)
	{
		cout << "Model could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (_vertexBuffer)
			free(_vertexBuffer);

		if (_indexBuffer)
			free(_indexBuffer);

		if (actualModel)
			actualModel->release();
		return-1;
	}
	return 0;
}

HRESULT Model::loadModelAssimp(ID3D11Device *device, const std::wstring& filename)
{
	ExtendedVertexStruct *_vertexBuffer = nullptr;
	uint32_t *_indexBuffer = nullptr;

	Assimp::Importer importer;
	std::wstring w(filename); 
	std::string filename_string(w.begin(), w.end());
	// Get filename extension
	wstring ext = filename.substr(filename.length() - 4);
	//if (0 == ext.compare(L".obj") || 0 == ext.compare(L".gsf"))// || 0 == ext.compare(L".3ds")
	//printf("OBJ\n", ext);

	try
	{
		const aiScene* scene = importer.ReadFile(filename_string, aiProcess_PreTransformVertices| aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
		//printf("Root transform=",scene->mRootNode->)


		if (!scene)
		{
			printf("Couldn't load model - Error Importing Asset");
			return false;
		}

		numMeshes = scene->mNumMeshes;

		if (numMeshes == 0)
			throw exception("Empty model loaded");

		uint32_t numVertices = 0;
		uint32_t numIndices = 0;
		//printf("Num Meshes %d\n", scene->mNumMeshes);
		for (uint32_t k = 0; k < numMeshes; ++k)
		{
			aiMesh* mesh = scene->mMeshes[k];
			//Store base vertex index;
			baseVertexOffset.push_back(numVertices);
			// Increment vertex count
			numVertices += mesh->mNumVertices;
			// Store num indices for current mesh
			indexCount.push_back(mesh->mNumFaces * 3);
			numIndices += mesh->mNumFaces * 3;
			//printf("Mesh %d\n", k);
			//printf("numVertices %d\n", mesh->mNumVertices);
			//printf("numIndices %d\n", mesh->mNumFaces);
		}

			// Create vertex buffer
		_vertexBuffer = (ExtendedVertexStruct*)malloc(numVertices * sizeof(ExtendedVertexStruct));

			if (!_vertexBuffer)
				throw exception("Cannot create vertex buffer");

			// Create index buffer
			_indexBuffer = (uint32_t*)malloc(numIndices * sizeof(uint32_t));

			if (!_indexBuffer)
				throw exception("Cannot create index buffer");

			// Copy vertex data into single buffer
			ExtendedVertexStruct *vptr = _vertexBuffer;
			uint32_t *indexPtr = _indexBuffer;

			for (uint32_t i = 0; i < numMeshes; ++i) 
			{

				aiMesh* mesh = scene->mMeshes[i];
				//printf("Mesh %d\n", i);
				//if (mesh->HasNormals())
				//	printf("has normals");
				uint32_t j = 0;
				for ( j = 0; j < mesh->mNumFaces; ++j)		
				{
					const aiFace& face = mesh->mFaces[j];
					for (int k = 0; k < 3; ++k)
					{
						int VIndex = baseVertexOffset[i] + face.mIndices[k];
						aiVector3D pos = mesh->mVertices[face.mIndices[k]];
						aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
						aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
						//Flip normal.x for OBJ & GSF (might be required for other files too?)
						if (0 == ext.compare(L".obj") || 0 == ext.compare(L".gsf"))// || 0 == ext.compare(L".3ds")
						{
							normal.x = -normal.x;

							
							pos.x = -pos.x;
						}
						vptr[VIndex].pos = XMFLOAT3(pos.x, pos.y, pos.z);
						vptr[VIndex].normal = XMFLOAT3(normal.x, normal.y, normal.z);
						vptr[VIndex].texCoord = XMFLOAT2(uv.x, 1-uv.y);
						vptr[VIndex].matDiffuse = material->getColour()->diffuse;//XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
						vptr[VIndex].matSpecular = material->getColour()->specular;// XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
						indexPtr[0] = face.mIndices[k];
						indexPtr++;
						/*printf("index %d = %d \n", j * 3 + k, face.mIndices[k]);*/
					}
				}//for each face
				//printf("faces %d\n", j);
			}//for each mesh

	
			// Setup DX vertex buffer interfaces
			D3D11_BUFFER_DESC vertexDesc;
			D3D11_SUBRESOURCE_DATA vertexData;

			ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
			ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

			vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexDesc.ByteWidth = numVertices * sizeof(ExtendedVertexStruct);
			vertexData.pSysMem = _vertexBuffer;

			HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

			if (!SUCCEEDED(hr))
				throw exception("Vertex buffer cannot be created");

			// Setup index buffer
			D3D11_BUFFER_DESC indexDesc;
			D3D11_SUBRESOURCE_DATA indexData;

			ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
			ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));

			indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexDesc.ByteWidth = numIndices * sizeof(uint32_t);
			indexData.pSysMem = _indexBuffer;

			hr = device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);

			if (!SUCCEEDED(hr))
				throw exception("Index buffer cannot be created");

			// Dispose of local resources
			if (_vertexBuffer)
			free(_vertexBuffer);
			if (_vertexBuffer)
			free(_indexBuffer);

			//printf("done\n");

		}
		catch (exception& e)
		{
			cout << "Model could not be instantiated due to:\n";
			cout << e.what() << endl;

			if (_vertexBuffer)
				free(_vertexBuffer);

			if (_indexBuffer)
				free(_indexBuffer);

			return-1;
		}

	return 0;
}

//Model::Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *tex_view, Material *_material) {
//
//	Num_Textures = 1;
//	load(device, _effect, filename,  _material);
//	// Setup texture interfaces
//	textureResourceViewArray[0] = tex_view;
//	if (textureResourceViewArray[0])
//		textureResourceViewArray[0]->AddRef();
//
//
//}
//
//Model::Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *_tex_view_array[], int _num_textures, Material *_material) {
//
//	load(device, _effect, filename,  _material);
//	setTextures(0,_num_textures, _tex_view_array);
//	//Num_Textures = min(MAX_TEXTURES, _num_textures);
//	//for (int i = 0; i < Num_Textures; i++)
//	//	textureResourceViewArray[i] = _tex_view_array[i];
//	
//
//
////}
//void Model::setTextures(int _start_slot, int _num_textures, ID3D11ShaderResourceView *_tex_view_array[]){
//	if (_start_slot > MAX_TEXTURES - 1 | _start_slot <0 | _num_textures<1)return;
//	int num_textures = min(_num_textures + _start_slot, MAX_TEXTURES) - _start_slot;
//	Num_Textures = max(num_textures + _start_slot, Num_Textures);
//
//	for (int i = _start_slot; i < num_textures + _start_slot; i++)
//		textureResourceViewArray[i] = _tex_view_array[i - _start_slot];
//};



//
//XMMATRIX Model::update(ID3D11DeviceContext *context, Camera *camera, double time) {
//	// //Update castle cBuffer
//	cBufferCPU->worldMatrix = update(time);
//	cBufferCPU->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferCPU->worldMatrix));
//	cBufferCPU->WVPMatrix = cBufferCPU->worldMatrix*camera->getViewMatrix() * *(camera->getProjMatrix());
//	mapCbuffer(context, cBufferCPU, cBufferGPU,sizeof(CBufferModel));
//	return worldMatrix;
//}
//
//XMMATRIX Model::update(ID3D11DeviceContext *context, Camera *camera) {
//	// //Update castle cBuffer
//	cBufferCPU->worldMatrix = worldMatrix;
//	cBufferCPU->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferCPU->worldMatrix));
//	cBufferCPU->WVPMatrix = cBufferCPU->worldMatrix*camera->getViewMatrix() * *(camera->getProjMatrix());
//	mapCbuffer(context, cBufferCPU, cBufferGPU, sizeof(CBufferModel));
//	return worldMatrix;
//}
//void Model::renderSimp(ID3D11DeviceContext *context) {
//
//
//
//	// Validate Model before rendering (see notes in constructor)
//	if (!context || !vertexBuffer || !indexBuffer || !effect)
//		return;
//
//	// Set vertex layout
//	context->IASetInputLayout(effect->getVSInputLayout());
//
//	// Set Model vertex and index buffers for IA
//	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
//	UINT vertexStrides[] = { sizeof(DXVertexExt) };
//	UINT vertexOffsets[] = { 0 };
//
//	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
//	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set primitive topology for IA
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//
//	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
//	if (textureResourceViewArray[0] && sampler) {
//
//		context->PSSetShaderResources(0, Num_Textures, textureResourceViewArray);
//
//	}
//	context->PSSetSamplers(0, 1, &sampler);
//	// Apply the cBuffer.
//	context->VSSetConstantBuffers(4, 1, &cBufferGPU);
//	context->PSSetConstantBuffers(4, 1, &cBufferGPU);
//
//
//	// Draw Model
//	for (uint32_t indexOffset = 0, i = 0; i < numMeshes; indexOffset += indexCount[i], ++i)
//		context->DrawIndexed(indexCount[i], indexOffset, baseVertexOffset[i]);
//}
//
//void Model::renderBasic(ID3D11DeviceContext *context) {
//
//
//
//	// Validate Model before rendering (see notes in constructor)
//	if (!context || !vertexBuffer || !indexBuffer || !effect)
//		return;
//
//	// Set vertex layout
//	context->IASetInputLayout(effect->getVSInputLayout());
//
//	// Set Model vertex and index buffers for IA
//	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
//	UINT vertexStrides[] = { sizeof(DXVertexExt) };
//	UINT vertexOffsets[] = { 0 };
//
//	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
//	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set primitive topology for IA
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//
//	context->PSSetSamplers(0, 1, &sampler);
//	// Apply the cBuffer.
//	context->VSSetConstantBuffers(4, 1, &cBufferGPU);
//	context->PSSetConstantBuffers(4, 1, &cBufferGPU);
//
//
//	// Draw Model
//	for (uint32_t indexOffset = 0, i = 0; i < numMeshes; indexOffset += indexCount[i], ++i)
//		context->DrawIndexed(indexCount[i], indexOffset, baseVertexOffset[i]);
//}