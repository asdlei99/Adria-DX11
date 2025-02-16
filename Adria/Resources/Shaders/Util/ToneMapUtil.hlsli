#ifndef _TONE_MAP_UTIL_
#define _TONE_MAP_UTIL_

static const float GAMMA = 2.2f;

float ColorToLuminance(float3 color)
{
    return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 LinearToneMapping(float3 color)
{
    color = clamp(color, 0., 1.);
    color = pow(color, 1. / GAMMA);
    return color;
}
float3 ReinhardToneMapping(float3 color)
{
    float luma = ColorToLuminance(color);
    float toneMappedLuma = luma / (1. + luma);
    if (luma > 1e-6)
        color *= toneMappedLuma / luma;
    
    color = pow(color, 1. / GAMMA);
    return color;
}
float3 ACESFilmicToneMapping(float3 color)
{
    color *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
}
float3 HableToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    float exposure = 2.;
    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, 1. / GAMMA);
    return color;
}

#endif