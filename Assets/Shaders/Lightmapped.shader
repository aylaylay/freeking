#ifdef VERTEX

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec2 uv1;
layout(location = 4) in vec2 uv2;

uniform GlobalUniforms
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
};

uniform mat4 modelMatrix;

out VertexData
{
	vec3 normal;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
} vert;

void main()
{
	vert.normal = normal;
	vert.uv0 = uv0;
	vert.uv1 = uv1;
	vert.uv2 = uv2;

	gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

uniform sampler2D diffuse;
uniform sampler2D lightmap;
uniform float brightness;
uniform float alphaMultiply;
uniform float alphaCutOff;

in VertexData
{
	vec3 normal;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
} vert;

out vec4 fragColor;

void main()
{
	vec4 textureColor = texture(diffuse, vert.uv0);

	if (textureColor.a < alphaCutOff)
	{
		discard;
	}

	vec3 lightmapColor0 = texture(lightmap, vert.uv1).rgb * 2.0;
	vec3 lightmapColor1 = texture(lightmap, vert.uv2).rgb * brightness;
	vec3 lightmapColor = lightmapColor0 + lightmapColor1;

	float gamma = 2.0;
	vec3 finalColor = textureColor.rgb * lightmapColor.rgb * lightmapColor.rgb;
	finalColor.rgb = pow(finalColor.rgb, vec3(1.0 / gamma));

	fragColor = vec4(finalColor.rgb, textureColor.a * alphaMultiply);
}

#endif
