#include "Sun.h"
#include "Canvas.h"

Sun::Sun(Canvas* c)
	:canvas(c)
{
	// Generate Shadow Map Framebuffer
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);	// Bind FBO for later DrawBuffer Binding

	// Generate Shadow Map Texture
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32,	// At first I was about to save storage space, but it turns out 16bit is more smooth than 32 bit in our case
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16,
		ShadowMapWidth, ShadowMapHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		/*GL_COMPARE_REF_TO_TEXTURE*/GL_NONE);	// http://stackoverflow.com/questions/21605146/shadow-mapping-shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);	// The functions is usded later when sampling inside fragment shader

	// Bind to drawbuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		shadowTexture, 0);
	glDrawBuffer(GL_NONE);	// This is a framebuffer state
	glReadBuffer(GL_NONE); 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Bind default FBO

	// Initiaze Properties
	//	UpdateSun(0);	// Give it a morning sun Don't do this because Camera hasn't been created yet
}

Sun::~Sun()
{
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &shadowTexture);
}

GLuint Sun::GetMap()
{
	return shadowTexture;
}

void Sun::UseFBO()
{
	// Use Framebuffer Object
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//glEnable(GL_DEPTH_TEST);

	// Clear Depth Buffer
	static GLfloat DefaultValue = 1.f;
	glClearBufferfv(GL_DEPTH, 0, &DefaultValue);
}

static void ColorMix(float* output, const float* reference1, const float* reference2, float percent)
{
	output[0] = glm::mix(reference1[0], reference2[0], percent);	// Returns x * (1.0 - a) + y * a
	output[1] = glm::mix(reference1[1], reference2[1], percent);
	output[2] = glm::mix(reference1[2], reference2[2], percent);
}

void Sun::UpdateSun(float sunTiming)
{
	// Assume the direction of sun from +X+Z->-X-Z
	// Assume color change from LightRed-LightYellow-LightCream-NotPureWhite(0.962, 0.962, 0.962)-VeryDimYellow(Moon) (The color should be very dim since it shouldn't contribute much to lighting) -- Deprecated
	// Assume sun goes from 0-360 degrees
	static const double sunDistance = 1000.;

	// Angle Divisions
	static const float SunAngles[] = { 0.f, 30.f, 60.f, 75.f, 105.f, 120.f, 165.f, 180.f};
	// Color Divisions: Here it describes the color of the sun, and how much light it contributes to the diffuse and specular term
	// Except during night moon's color doesn't match its light, otherwise we can assume the color of the sun mathces the strength of sunLight
	static const float SunColor_LightRed[3] = { 0.6f, 0.273f, 0.196f };	//  SunRise
	static const float SunColor_LightYellow[3] = { 0.923f, 0.837f, 0.581f};	// Morning; AfterNoon
	static const float SunColor_LightCream[3] = { 0.862f, 0.825f, 0.760f};	// Noon; AfterMidDay
	static const float SunColor_NotPureWhite[3] = { 0.825f, 0.825f, 0.825f};	// MidDay
	static const float SunColor_SunRed[3] = { 0.674f, 0.337f, 0.349f };	// Sunset
	static const float MoonColor_DimOrgange[3] = { 0.292f, 0.243f, 0.188f};
	// Sky: Here is describes the ambient light produced by the sky, not the actual color of the sky since they differ a lot
	// Also as a rule of thumn , the value/strength of the those colors shouldn't be over 50%
	static const float SkyColor_LightBlue[3] = { 0.292f, 0.340f, 0.383f};
	static const float SkyColor_VeryLightBlue[3] = { 0.274f, 0.335f, 0.361f};	// Very Light Blue
	static const float SkyColor_NotPureWhite[3] = { 0.367f, 0.403f, 0.408f};	// MidDay
	static const float SkyColor_PurpleBlue[3] = { 0.269f, 0.366f, 0.421f};	// Sunset
	static const float NightColor_LightBlue[3] = { 0.131f, 0.160f, 0.185f};

	// Angle in radians
	double  radians = sunTiming * 3.14159 / 180;

	// Calculate Sun Behavior
	sunLocation[0] = sunDistance * 0.707f * glm::cos(radians);	// X - Beginning Point: sqrt(2)/2 * sunDistance = 0.707 * sunDistance
	sunLocation[1] = glm::abs(sunDistance * glm::sin(radians));	// Y - Height, always positive
	sunLocation[2] = sunDistance * 0.707f * glm::cos(radians);	// Z
	if (sunTiming > 180)
	{
		sunLocation[0] = -sunLocation[0];
		sunLocation[1] = glm::abs(sunDistance * glm::sin(radians));
		sunLocation[2] = -sunLocation[2];
	}

	// Calculate Sun Color
	if (sunTiming < SunAngles[1])
	{
		float percent = sunTiming / SunAngles[1];

		ColorMix(sunColor, SunColor_LightRed, SunColor_LightYellow, percent);

		ColorMix(skyFillColor, SkyColor_LightBlue, SkyColor_LightBlue, percent);
	}
	else if (sunTiming < SunAngles[2])
	{
		float percent = (sunTiming - SunAngles[1]) / (SunAngles[2] - SunAngles[1]);
		ColorMix(sunColor, SunColor_LightYellow, SunColor_LightCream, percent);

		ColorMix(skyFillColor, SkyColor_LightBlue, SkyColor_VeryLightBlue, percent);
	}
	else if (sunTiming < SunAngles[3])
	{
		float percent = (sunTiming - SunAngles[2]) / (SunAngles[3] - SunAngles[2]);
		ColorMix(sunColor, SunColor_LightCream, SunColor_NotPureWhite, percent);

		skyFillColor[0] = SkyColor_VeryLightBlue[0];
		skyFillColor[1] = SkyColor_VeryLightBlue[1];
		skyFillColor[2] = SkyColor_VeryLightBlue[2];
	}
	else if (sunTiming < SunAngles[4])
	{
		float percent = (sunTiming - SunAngles[3]) / (SunAngles[4] - SunAngles[3]);
		ColorMix(sunColor, SunColor_NotPureWhite, SunColor_LightCream, percent);

		skyFillColor[0] = SkyColor_VeryLightBlue[0];
		skyFillColor[1] = SkyColor_VeryLightBlue[1];
		skyFillColor[2] = SkyColor_VeryLightBlue[2];
	}
	else if (sunTiming < SunAngles[5])
	{
		float percent = (sunTiming - SunAngles[4]) / (SunAngles[5] - SunAngles[4]);
		ColorMix(sunColor, SunColor_LightCream, SunColor_LightYellow, percent);

		ColorMix(skyFillColor, SkyColor_VeryLightBlue, SkyColor_LightBlue, percent);
	}
	else if (sunTiming < SunAngles[6])
	{
		float percent = (sunTiming - SunAngles[5]) / (SunAngles[6] - SunAngles[5]);
		ColorMix(sunColor, SunColor_LightYellow, SunColor_LightRed, percent);

		ColorMix(skyFillColor, SkyColor_LightBlue, SkyColor_PurpleBlue, percent);
	}
	else if (sunTiming < SunAngles[7])
	{
		float percent = (sunTiming - SunAngles[6]) / (SunAngles[7] - SunAngles[6]);

		ColorMix(sunColor, SunColor_SunRed, MoonColor_DimOrgange, percent);

		ColorMix(skyFillColor, SkyColor_PurpleBlue, NightColor_LightBlue, percent);
	}
	else
	{
		sunColor[0] = MoonColor_DimOrgange[0];
		sunColor[1] = MoonColor_DimOrgange[1];
		sunColor[2] = MoonColor_DimOrgange[2];

		skyFillColor[0] = NightColor_LightBlue[0];
		skyFillColor[1] = NightColor_LightBlue[1];
		skyFillColor[2] = NightColor_LightBlue[2];
	}

	// Version3
	float X = sunLocation[0], Y = sunLocation[1], Z = sunLocation[2];	// The location of the sun doesn't matter since we are in ortho, the only thing matters is the direction
	glm::vec3 camera = canvas->GetCamera()->GetWorldPosition();

	// Get Sun Rotation Direction
	glm::vec3 N = glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 N1 = glm::normalize(glm::vec3(-X, -Y, -Z));
	glm::vec3 rotationAxis = glm::cross(N, N1);
	float rotationAngle = glm::acos( glm::dot(N, N1) / (glm::length(N) * glm::length(N1)));

	glm::mat4 rotation = glm::rotate(glm::mat4(1), rotationAngle, rotationAxis);
	sunView = glm::inverse(glm::translate(glm::mat4(1), glm::vec3(X + camera.x, Y, Z + camera.z))*rotation);	// Let the sun move with camera: currently camera isn't exactly at (shadow) viewport center but that doesn't bother much

	float relativeRatio = (float)(canvas->viewportH) / (float)(canvas->viewportW);
	float sightWidth = canvas->GetSightDepth();
	sunProjection = glm::ortho<float>(-sightWidth, sightWidth, sightWidth * (-relativeRatio), sightWidth * relativeRatio, 500.f/*0.f*/, 3000.f);	//__Pending__
	// http://stackoverflow.com/questions/13199126/find-opengl-rotation-matrix-for-a-plane-given-the-normal-vector-after-the-rotat
	// Search KEy word: How to get a rotation matrix normal to a vector
}

// Alignment:
void* Sun::operator new (size_t size)
{
	return _aligned_malloc(size, 16);
}

void Sun::operator delete (void *p)
{
	_aligned_free(p);
}