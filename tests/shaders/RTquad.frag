#version 410 core

// Input from vertex shader
in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 outFragColor;

// Uniforms
uniform vec2 uResolution;
uniform sampler2D uPreviousFrameRT;
uniform float uTime;
uniform int uFrameIndex;

/********************************************************/
/* Helper math functions */
#define PI 3.1415926535897932385
#define MAX_FLOAT 9999
#define MAX_RECURSION 5
#define TAU 2. * PI
// Golden Ratio
#define PHI 1.61803398874989484820459

float seed = 0.25;

// random number generator

//https://stackoverflow.com/a/34276128
bool isnan(float x){
  return !(x > 0. || x < 0. || x == 0.);
}

// a variation of gold noise is used
// https://stackoverflow.com/a/28095165
// https://www.shadertoy.com/view/ltB3zD
// centered around [0-1] in gaussian
float Random (vec2 st) {
     return fract(tan(distance(st*PHI, st)*seed)*st.x);
}

vec2 Random2(float seed){
  return vec2(
    Random(vec2(seed-1.23, (seed+3.1)* 3.2)),
    Random(vec2(seed+12.678, seed - 5.8324))
    );
}

vec3 Random3(float seed){
  return vec3(
    Random(vec2(seed-0.678, seed-0.123)),
    Random(vec2(seed-0.3, seed+0.56)),
    Random(vec2(seed+0.1234, seed-0.523))
    );
}

vec3 RandomInUnitSphere(float seed) {
  vec2 tp = Random2(seed);
  float theta = tp.x * TAU;
  float phi = tp.y * TAU;
  vec3 p = vec3(sin(theta) * cos(phi), sin(theta)*sin(phi), cos(theta));

  return normalize(p);
}

vec3 RandomUnit(float seed){
    vec2 rand = Random2(seed);
    float a = rand.x * TAU;
    float z = (2. * rand.y) - 1.;
    float r = sqrt(1. - z*z);
    return vec3(r*cos(a), r*sin(a), z);
}


float Deg2Rad(float deg)
{
  return deg*PI / 180.;
}

/********************************************************/
/* Type Definitions */
/* Ray */
struct Ray
{
    vec3 origin; // Beginning point of the ray
    vec3 dir;	 // Direction of the ray
};

/* Geometry */
struct Sphere
{
  	vec3 center;
  	float radius;
};

/* Abstraction of hittable objects */
struct HitInfo
{
	vec3 point;
	vec3 normal;
	float t;
	bool isFrontFace;
};
/********************************************************/
// Environment
vec3 SkyColor = vec3(0.5, 0.7, 1.0);
bool drawNormals = true;

/********************************************************/
/* Geometry Hit Functions */

// Gets the positon of ray at any point in the space using the parametric equation of st. line
vec3 RayAt(Ray r, float t)
{
	return r.origin + r.dir*t;
}

// Sphere Hit
bool RayHasHitSphere(Sphere s, Ray r, inout HitInfo hitInfo)
{
    vec3 oc = r.origin - s.center;
    float a = dot(r.dir, r.dir);
    float halfB = dot(oc,r.dir);
    float c = dot(oc, oc) - s.radius * s.radius;
    float d = halfB * halfB - a * c;
    if(d < 0.){ return false; }

	// check if we are inside the sphere and the closest intersection is behind us.
    float t1 = (-halfB - sqrt(d));
    float t2 = (-halfB + sqrt(d));
    float t = t1 < 0.05 ? t2 : t1;

	// Get the hit record deets
	vec3 hitPoint = RayAt(r, t);
	vec3 hitNormal = hitPoint - s.center;

    // if it is the front face, the ray is inside the sphere, so invert the normal
    bool frontFace = dot(r.dir, hitNormal) > 0.0;
	hitNormal = frontFace ? -hitNormal : hitNormal;
  	// Normalize the normal length
  	hitNormal /= s.radius;

  	// If not roots return no hit
	if(t < 0.05 || t > 1000.)
  	return false;

    // If it hit something return true along with the hit data of the hit
    hitInfo = HitInfo(hitPoint, hitNormal, t, frontFace);

	return true;
}

/********************************************************/
// Ray hit Functions

// Ray Caster with the objects in the world to render them
bool RayCastWorld(const in Ray r, inout HitInfo hitInfo)
{
	// Draw the red sphere first (with that rays that hit that object)
	Sphere redSphere = Sphere(vec3(0.0, 0.0, -1.0), 0.4);
	// Now draw the plane as a very huge sphere
	Sphere ground = Sphere(vec3(0.,-100.5, -1.), 100.);

	// Check if the ray has hit anything , if yes return true with the hitInfo
	bool hasHitAnything = false;
	hasHitAnything = RayHasHitSphere(ground, r, hitInfo) || hasHitAnything;
	hasHitAnything = RayHasHitSphere(redSphere, r, hitInfo) || hasHitAnything;
	return hasHitAnything;
}

// Gets the color of the ray being incident
vec3 RayColor(Ray r)
{
	// Gets the direcition of the ray in NDC
	vec3 unitDir = normalize(r.dir);

    HitInfo hitInfo;
    vec3 col = vec3(1.0);

    // Add uniforms to change render mode
    /*
    // Check if we have hit any hittable object, if not render the sky instead
	bool hasHitAnything = RayCastWorld(r, hitInfo);
    if(hasHitAnything){
        return 0.5 * (hitInfo.normal + vec3(1.0));
    }
    */

    // Diffuse shading
    for(int i = 0; i < MAX_RECURSION; i++)
    	{
    		bool hasHit = RayCastWorld(r, hitInfo);
    		if(hasHit)
    		{
    			vec3 jitter = RandomInUnitSphere(seed);
          		if(isnan(jitter.r) || isnan(jitter.g) || isnan(jitter.b)){
            		jitter = vec3(0.);
          		}
          		col *= 0.5;
          		vec3 target = (hitInfo.point + hitInfo.normal + jitter);
          		r.origin = hitInfo.point;
          		r.dir = normalize(target - hitInfo.point);
    		}
    		else
    		{
    			// Gets the direction of the ray in NDC
    			vec3 unitDir = normalize(r.dir);
    			/*
    	 		* If it doesn't hit anything then draw the Sky
    	 		* Use the parametric equation to get a point in ray and
    	 		* draw a gradient of the color over the screen
    	 		*/
    			// TODO: How is this t determined in the next line?
    			float t = 0.5 * (unitDir.y + 1.0);
    			//return (1.0 - t)*vec3(1.0) + t*vec3(0.5, 0.7, 1.0);
    			col *= mix(vec3(1.0), SkyColor, t);
    			return col;
    		}
    	}
	return col;
}

/********************************************************/
// Camera Systems
struct Camera
{
	// Camera world space properties
	vec3 origin;
	vec3 up;
	vec3 right;
	vec3 lowerLeftCorner;
};

// Creates the camera for the scence
Camera CreateCamera()
{
	// Camera Viewport properties
	float aspectRatio = (uResolution.x / uResolution.y);
	float viewportHeight = 2.0f;
	float viewportWidth = viewportHeight * aspectRatio;
	float focalLength = 1.0;

	// Create the camera vectors
	vec3 origin = vec3(0.0);
	vec3 camUp = vec3(0.0, viewportHeight, 0.0);
	vec3 camRight = vec3(viewportWidth, 0.0, 0.0);
	vec3 lowerLeft = origin - camUp / 2 - camRight / 2 - vec3(0.0, 0.0, focalLength);

	// Create the camera struct the with initial data
	return Camera(origin, camUp, camRight, lowerLeft);
}

// Gets the Ray from the eye of the camera to the pixel on the screen
Ray GetRayFromCamera(Camera c, vec2 uv)
{
	Ray r = Ray(c.origin, normalize(c.lowerLeftCorner + uv.x * c.right + uv.y * c.up - c.origin));
	return r;
}
/********************************************************/
out vec4 OutFragColor;

void main()
{

    // Creating the UV map texture
    vec2 uv = gl_FragCoord.xy/uResolution;

    // Create the scene camera
    Camera cam = CreateCamera();

    // new seed every frame
  	seed = Random(gl_FragCoord.xy * (mod(uTime, 100.)));
  	if(isnan(seed)){
    	seed = 0.25;
  	}

    vec2 jitter = (2. * Random2(seed)) - 1.;
  	vec2 st = uv + jitter * 0.001;
  	// check for NaN leakage
  	if(isnan(st.x) || isnan(st.y)){
    	st = uv;
  	}

    // Get the ray from cam to uv pixel
    Ray r = GetRayFromCamera(cam, st);

    vec3 col = RayColor(r);

    // Min it with the last frame render
    // average the frames together
    vec3 lastFrameColor = texture(uPreviousFrameRT, uv).rgb;
    col = mix(lastFrameColor, col, 1.0f / float(uFrameIndex+1));

    // show the result
    outFragColor = vec4(col, 1.0f);
}
