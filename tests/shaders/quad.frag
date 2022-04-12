#version 330 core

out vec3 position;

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

uniform sampler2D quad_texture;

// a pixel value multiplier of light before tone mapping and sRGB
const float c_exposure = 0.5f;

out vec4 outFragColor;


// SRGB-Linear Conversion Functions
vec3 LessThan(vec3 f, float value)
{
   return vec3(
       (f.x < value) ? 1.0f : 0.0f,
       (f.y < value) ? 1.0f : 0.0f,
       (f.z < value) ? 1.0f : 0.0f);
}

vec3 LinearToSRGB(vec3 rgb)
{
   rgb = clamp(rgb, 0.0f, 1.0f);

   return mix(
       pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
       rgb * 12.92f,
       LessThan(rgb, 0.0031308f)
   );
}

vec3 SRGBToLinear(vec3 rgb)
{
   rgb = clamp(rgb, 0.0f, 1.0f);

   return mix(
       pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
       rgb / 12.92f,
       LessThan(rgb, 0.04045f)
   );
}

// Tonemapping
// ACES tone mapping curve fit to go from HDR to LDR
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
   float a = 2.51f;
   float b = 0.03f;
   float c = 2.43f;
   float d = 0.59f;
   float e = 0.14f;
   return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}

/*
FAST APPROXIMATION OF https://www.shadertoy.com/view/3dd3Wr

[
This project did NOT use any code from the /\ above, I was creating this
whilst comparing its visuals to the above project
]

Boi if anybody uses this script you better not change the name of the function

By: Sir Bird / Zerofile

*/

#define SAMPLES 80  // HIGHER = NICER = SLOWER
#define DISTRIBUTION_BIAS 0.6 // between 0. and 1.
#define PIXEL_MULTIPLIER  1.5 // between 1. and 3. (keep low)
#define INVERSE_HUE_TOLERANCE 20.0 // (2. - 30.)

#define GOLDEN_ANGLE 2.3999632 //3PI-sqrt(5)PI

#define pow(a,b) pow(max(a,0.),b) // @morimea

mat2 sample2D = mat2(cos(GOLDEN_ANGLE),sin(GOLDEN_ANGLE),-sin(GOLDEN_ANGLE),cos(GOLDEN_ANGLE));

vec3 sirBirdDenoise(sampler2D imageTexture, in vec2 uv, in vec2 imageResolution) {

    vec3 denoisedColor           = vec3(0.);

    const float sampleRadius     = sqrt(float(SAMPLES));
    const float sampleTrueRadius = 0.5/(sampleRadius*sampleRadius);
    vec2        samplePixel      = vec2(1.0/imageResolution.x,1.0/imageResolution.y);
    vec3        sampleCenter     = texture(imageTexture, uv).rgb;
    vec3        sampleCenterNorm = normalize(sampleCenter);
    float       sampleCenterSat  = length(sampleCenter);

    float  influenceSum = 0.0;
    float brightnessSum = 0.0;

    vec2 pixelRotated = vec2(0.,1.);

    for (float x = 0.0; x <= float(SAMPLES); x++) {

        pixelRotated *= sample2D;

        vec2  pixelOffset    = PIXEL_MULTIPLIER*pixelRotated*sqrt(x)*0.5;
        float pixelInfluence = 1.0-sampleTrueRadius*pow(dot(pixelOffset,pixelOffset),DISTRIBUTION_BIAS);
        pixelOffset *= samplePixel;

        vec3 thisDenoisedColor =
            texture(imageTexture, uv + pixelOffset).rgb;

        pixelInfluence      *= pixelInfluence*pixelInfluence;
        /*
            HUE + SATURATION FILTER
        */
        pixelInfluence      *=
            pow(0.5+0.5*dot(sampleCenterNorm,normalize(thisDenoisedColor)),INVERSE_HUE_TOLERANCE)
            * pow(1.0 - abs(length(thisDenoisedColor)-length(sampleCenterSat)),8.);

        influenceSum += pixelInfluence;
        denoisedColor += thisDenoisedColor*pixelInfluence;
    }

    return denoisedColor/influenceSum;

}

void main()
{

    vec3 color = texture(quad_texture, vs_in.texCoords).rgb;

    color = sirBirdDenoise(quad_texture, vs_in.texCoords, vec2(500, 500)).rgb;

    // apply exposure (how long the shutter is open)
    color *= c_exposure;

    // convert unbounded HDR color range to SDR color range
    color = ACESFilm(color);

    // convert from linear to sRGB for display
    color = LinearToSRGB(color);

    outFragColor = vec4(color, 1.0f);
}
