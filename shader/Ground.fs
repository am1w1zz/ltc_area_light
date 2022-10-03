#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
in vec4 WorldPos;

uniform sampler2D LTC_1;
uniform sampler2D LTC_2;

uniform vec3  CameraPos;
uniform vec3  DiffuseColor;
uniform vec3  SpecularColor;
uniform vec3  PolygonalLightVertexPos[4];
uniform float Roughness;
uniform float Intensity;

const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;
const float pi = 3.14159265;

vec3 PowVec3(vec3 v, float p) {
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

const float gamma = 2.2;
vec3 ToLinear(vec3 v) { 
	return PowVec3(v, gamma);
}

void ClipQuadToHorizon(inout vec3 L[5], out int n)
{
    // detect clipping config
    int config = 0;
    if (L[0].z > 0.0) config += 1;
    if (L[1].z > 0.0) config += 2;
    if (L[2].z > 0.0) config += 4;
    if (L[3].z > 0.0) config += 8;

    // clip
    n = 0;

    if (config == 0)
    {
        // clip all
    }
    else if (config == 1) // V1 clip V2 V3 V4
    {
        n = 3;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 2) // V2 clip V1 V3 V4
    {
        n = 3;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 3) // V1 V2 clip V3 V4
    {
        n = 4;
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
        L[3] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 4) // V3 clip V1 V2 V4
    {
        n = 3;
        L[0] = -L[3].z * L[2] + L[2].z * L[3];
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
    }
    else if (config == 5) // V1 V3 clip V2 V4) impossible
    {
        n = 0;
    }
    else if (config == 6) // V2 V3 clip V1 V4
    {
        n = 4;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 7) // V1 V2 V3 clip V4
    {
        n = 5;
        L[4] = -L[3].z * L[0] + L[0].z * L[3];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 8) // V4 clip V1 V2 V3
    {
        n = 3;
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
        L[1] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] =  L[3];
    }
    else if (config == 9) // V1 V4 clip V2 V3
    {
        n = 4;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[2].z * L[3] + L[3].z * L[2];
    }
    else if (config == 10) // V2 V4 clip V1 V3) impossible
    {
        n = 0;
    }
    else if (config == 11) // V1 V2 V4 clip V3
    {
        n = 5;
        L[4] = L[3];
        L[3] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 12) // V3 V4 clip V1 V2
    {
        n = 4;
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
    }
    else if (config == 13) // V1 V3 V4 clip V2
    {
        n = 5;
        L[4] = L[3];
        L[3] = L[2];
        L[2] = -L[1].z * L[2] + L[2].z * L[1];
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
    }
    else if (config == 14) // V2 V3 V4 clip V1
    {
        n = 5;
        L[4] = -L[0].z * L[3] + L[3].z * L[0];
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
    }
    else if (config == 15) // V1 V2 V3 V4
    {
        n = 4;
    }

    if (n == 3)
        L[3] = L[0];
    if (n == 4)
        L[4] = L[0];
}

// vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
// {
//     float x = dot(v1, v2);
//     float y = abs(x);

//     float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
//     float b = 3.4175940 + (4.1616724 + y)*y;
//     float v = a / b;

//     float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

//     return cross(v1, v2)*theta_sintheta;
// }

// float IntegrateEdge(vec3 v1, vec3 v2)
// {
//     return IntegrateEdgeVec(v1, v2).z;
// }

vec3 IntegrateEdge(vec3 vVertex1, vec3 vVertex2)
{
	float CosTheta = dot(vVertex1, vVertex2);
	float Theta = acos(CosTheta);
	return cross(vVertex1, vVertex2) * ((Theta > 0.001) ? Theta / sin(Theta) : 1.0);
}

vec3 LTC_Evaluate(vec3 Normal, vec3 View, vec3 FragPos, mat3 LTC, vec3 LightPos[4]) {
	vec3 Tangent = normalize(View - Normal * dot(View, Normal));
	vec3 Bitangent = cross(Normal, Tangent);
	LTC = LTC * transpose(mat3(Tangent, Bitangent, Normal));
	vec3 L[5];
	L[0] = LTC * (LightPos[0] - FragPos);
    L[1] = LTC * (LightPos[1] - FragPos);
    L[2] = LTC * (LightPos[2] - FragPos);
    L[3] = LTC * (LightPos[3] - FragPos);
	vec3 VSum = vec3(0.0);
	// float Sum = 0.0;
	int n = 0;
	ClipQuadToHorizon(L, n);
	if(n == 0) {
		return vec3(0.0);
	}
	L[0] = normalize(L[0]);
	L[1] = normalize(L[1]);
	L[2] = normalize(L[2]);
	L[3] = normalize(L[3]);
	L[4] = normalize(L[4]);
	VSum += IntegrateEdge(L[0], L[1]);
	VSum += IntegrateEdge(L[1], L[2]);
	VSum += IntegrateEdge(L[2], L[3]);
	if (n >= 4){
		VSum += IntegrateEdge(L[3], L[4]);
	}
	if (n == 5) {
		VSum += IntegrateEdge(L[4], L[0]);
	}
	float Sum = max(0.0, VSum.z);
	// float Sum = abs(VSum.z);
	return vec3(Sum);
}

vec2 LutCoords(float NdotV, float Roughness) {
	float ClampedNdotV = clamp(NdotV, 0.0, 1.0);
	vec2 UV = vec2(Roughness, sqrt(1.0 - ClampedNdotV));
	UV = UV*LUT_SCALE + LUT_BIAS;
	return UV;
    // float Theta = acos(NdotV);
    // vec2 Coords = vec2(Roughness, Theta/(0.5 * pi));
    // const float LUT_SIZE = 32.0;
    // // scale and bias coordinates, for correct filtered lookup
    // Coords = Coords * (LUT_SIZE - 1.0)/LUT_SIZE + 0.5/LUT_SIZE;
    // return Coords;
}

void main()
{
	
	vec3 Normal = vec3(0.0, 1.0, 0.0);
	vec3 ViewDirection  = normalize(CameraPos - WorldPos.xyz);
	vec2 UV = LutCoords(dot(Normal, ViewDirection), Roughness);
	vec4 LTC1 = texture(LTC_1, UV);
	vec4 LTC2 = texture(LTC_2, UV);
	vec3 DiffColor = ToLinear(DiffuseColor);
	vec3 SpecColor = ToLinear(SpecularColor);

	mat3 Minv = mat3(
        	vec3(LTC1.x, 0, LTC1.y),
        	vec3(  0,  1,    0),
        	vec3(LTC1.z, 0, LTC1.w)
        );
 	vec3 Spec = LTC_Evaluate(Normal, ViewDirection, WorldPos.xyz, Minv, PolygonalLightVertexPos);
	vec3 Diff = LTC_Evaluate(Normal, ViewDirection, WorldPos.xyz, mat3(1), PolygonalLightVertexPos);
    // BRDF shadowing and Fresnel
    Spec *= SpecColor*LTC2.x + (1.0 - SpecColor)*LTC2.y;
	Diff *= DiffColor;
	vec3 Color = vec3(Intensity) * (Spec + Diff);
	Color = Color / (Color + vec3(1.0));
  	Color = pow(Color, vec3(1.0/2.2)); 
  	FragColor = vec4(Color,1.0);
}