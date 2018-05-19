//const GLchar* Shaders::vertexShaders[NumberOfPrograms] =
//{
//	// Text Program_VertexShader
//	"#version 430 core\n"
//	"layout(location = 0) in vec3 position;\n"	// 3D position is necessary since we will use it in 3D space as well
//	"layout(location = 1) in vec2 textureCoord;\n"
//	"layout(location = 2) in int mapID;\n"
//	"layout(location = 3) uniform mat4x4 MVP;\n"
//	"out vec2 tc;\n"
//	"flat out int id;\n"
//	"void main()\n"
//	"{\n"
//	"	gl_Position = MVP * vec4(position, 1);\n"
//	"	tc = textureCoord;"
//	"	id = mapID;\n"
//	"}\n",
//
//	// Shape Program
//	"#version 430 core\n"
//	"layout(location = 0) in vec3 position;\n"
//	"layout(location = 1) uniform mat4x4 MVP;\n"
//	"void main()\n"
//	"{\n"
//	"	gl_Position = MVP * vec4(position, 1);\n"
//	"}\n",
//
//	// SimpleTextureProgram
//	"#version 430 core\n"
//	"layout(location = 0) in vec3 position;\n"	// 3D position is necessary since we will use it in 3D space as well
//	"layout(location = 1) in vec2 texCoord;\n"
//	"layout(location = 2) uniform mat4x4 MVP;\n"
//	"out vec2 tc;\n"
//	"void main()\n"
//	"{\n"
//	"	gl_Position = MVP * vec4(position, 1);\n"
//	"	tc = texCoord;"
//	"}\n",
//
//	// PhoneShadingProgram
//};
//
//const GLchar* Shaders::fragmentShaders[NumberOfPrograms] =
//{
//	// Text Program_FragmentShader
//	"#version 430 core\n"
//	"layout(location = 4) uniform vec3 color;\n"
//	"layout(binding = 0) uniform sampler2D texture0;"	// Provide Upto 2 texture bindings
//	"layout(binding = 1) uniform sampler2D texture1;"
//	"flat in int id;\n"
//	"in vec2 tc;\n"
//	"out vec4 fcolor;\n"
//	"void main()\n"
//	"{\n"
//	"switch(id)\n"
//	"{\n"
//	"case 0:\n"
//	"	fcolor = vec4(color.rgb,1) * texture(texture0,tc).r;\n"
//	"	break;\n"
//	"case 1:\n"
//	"	fcolor = vec4(color.rgb,1) * texture(texture1,tc).r;\n"
//	"	break;\n"
//	"default:\n"
//	"	fcolor = vec4(color.rgb,1);\n"
//	"	break;\n"
//	"}\n"
//	"}\n",
//
//	// Shape Program
//	"#version 430 core\n"
//	"layout(location = 2) uniform vec3 color;\n"
//	"layout(location = 3) uniform float timeScale;\n"
//	"out vec4 fcolor;\n"
//	"void main()\n"
//	"{\n"
//	"	fcolor = vec4(color.rgb, timeScale);\n"
//	"}\n",
//
//	// SimpleTextureProgram
//	"#version 430 core\n"
//	"layout(binding = 0) uniform sampler2D texture0;"	// Provide 1 texture bindings
//	"in vec2 tc;\n"
//	"out vec4 fcolor;\n"
//	"void main()\n"
//	"{\n"
//	"fcolor = texture(texture0,tc);\n"
//	"}\n",
//
//	// PhoneShadingProgram
//	/* In order to provide the ability to use either a color or a texture, we can either: provide different combinations of shaders,
//	which means provide a bunch of meaningless shaders;
//	Or provide mask texture as color inputs(i.e. the shader expect both a color and a texture, while when only color is needed we can
//	provide a white texture, when texture is needed we can provide a white color).*/
//	""
//};