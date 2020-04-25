#version 420

in vec2 v_TexCoord;
in vec4 v_VertexColor;

out vec4 out_FragColor;

uniform sampler2D u_Texture;
uniform float u_Edge0;
uniform float u_Edge1;

void main()
{
	// TODO: Use dFdx(v_TexCoord.x), dFdy(v_TexCoord.y) to shift the edge when font is far away/small
	float sdfValue = texture(u_Texture, v_TexCoord).r;
	float alpha = smoothstep(u_Edge0, u_Edge1, sdfValue);
	out_FragColor = vec4(v_VertexColor.rgb, alpha);
}
