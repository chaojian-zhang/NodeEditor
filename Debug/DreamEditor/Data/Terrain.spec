Shader = 3
ShapeMesh = "Terrain.obj"

GroundColor={0.1, 0.1, 0.1}

PointLightColor = {0.25, 0.25, 0.25}
PointLightLocation = {0.0, 10.0, 0.0}

DiffuseColor = {1.0, 1.0, 1.0}
DiffuseTexture = "TerrainD.jpg"

SpecularPower = 32.0	// Not useful since we have specular texture -> Actually we used this to control the Maximum Multiply value
//SpecularPower = 0.f
SpecularTexture = "TerrainS.jpg" // Currently our scene is too bright thus Specular look overbright when specularity is 0
// Or actually it is because this specualr map isn't well tunned

EmissiveColor = {0.0, 0.0, 0.0}
NormalTexture = "TerrainN.jpg"

// We cannot embed a sky dome in the object because of clipping distance, and we shouldn't make that too high