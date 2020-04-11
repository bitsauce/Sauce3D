in vec2 v_TexCoord;
in vec4 v_VertexColor;

out vec4 out_FragColor;

uniform sampler2D u_Texture;

void main()
{
	float sdfValue = texture(u_Texture, v_TexCoord*2).r;
	float alpha = smoothstep(0.0f, 0.5f, 1.0f - sdfValue);
	out_FragColor = vec4(v_VertexColor.rgb, alpha);
}
