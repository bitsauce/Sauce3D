in vec2 v_TexCoord;
in vec3 v_FragWorldPos;
in vec3 v_Normal;
		
out vec4 out_FragColor;

#define MAX_LIGHTS 8

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};

struct PointLight
{
	vec3 position;
	vec3 color;
	float radius;
};
			
uniform sampler2D u_Texture;
uniform DirectionalLight u_DirLight;
uniform int u_NumPointLights;
uniform PointLight u_PointLight[MAX_LIGHTS];

void main()
{
	vec3 albedo = texture(u_Texture, v_TexCoord).rgb;
	vec3 normal = normalize(v_Normal);
	
	vec3 finalColor = vec3(0.f, 0.f, 0.f);
	for (int i = 0; i < u_NumPointLights; i++)
	{
		const PointLight pointLight = u_PointLight[i];
		vec3 lightDirection = normalize(pointLight.position - v_FragWorldPos);
		float diff = max(dot(normal, lightDirection), 0.f);
		vec3 diffuse = diff * pointLight.color;
		finalColor += diffuse * albedo;
	}

	finalColor += albedo * 0.25;
	out_FragColor = vec4(clamp(finalColor, 0.f, 1.f), 1.f);
}