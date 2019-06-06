#include "stdafx.h"
#include "Material.h"


Material::Material()
{
	colour.emissive = XMCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	colour.ambient = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	colour.diffuse = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	colour.specular = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}


Material::~Material()
{
}
