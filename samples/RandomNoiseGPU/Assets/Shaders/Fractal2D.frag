in vec2 v_TexCoord;
out vec4 out_FragColor;
uniform float u_Time;

int hash(int x)
{
    x += (x << 10U);
    x ^= (x >>  6U);
    x += (x <<  3U);
    x ^= (x >> 11U);
    x += (x << 15U);
    return x;
}

float rand(float x)
{
	return 0.5 * (float(hash(int(x * 1280.241223))) * (1.0 / 2147483647.0)) + 0.5;
}

float rand(vec2 pos)
{
	return 0.5 * (float(hash((int(pos.x * 1280.241223) << 16U) + int(pos.y * 720.241223))) * (1.0 / 2147483647.0)) + 0.5;
}

float fractalNoise1D(int octaves, float x)
{
	float value = 0.0;
	float amplitude = 0.5 / (1.0 - pow(2.0, -float(octaves)));
	for(int i = 0; i < octaves; i++)
	{
		float j = floor(x);
		float f = smoothstep(0.0, 1.0, fract(x));
		value += amplitude * mix(rand(j), rand(j + 1.0), f);
		x *= 2.0;
		amplitude *= 0.5;
	}
	return value;
}

float fractalNoise2D(int octaves, vec2 pos)
{
	float value = 0.0;
	float amplitude = 0.5 / (1.0 - pow(2.0, -float(octaves)));
	for(int i = 0; i < octaves; i++)
	{
		vec2 j = floor(pos);
		vec2 f = smoothstep(0.0, 1.0, fract(pos));
		value += amplitude * mix(
								mix(rand(j + vec2(0.0, 0.0)),
									rand(j + vec2(1.0, 0.0)), f.x),
								mix(rand(j + vec2(0.0, 1.0)),
									rand(j + vec2(1.0, 1.0)), f.x),
								f.y);
		pos *= 2.0;
		amplitude *= 0.5;
	}
	return value;
}

void main()
{
	out_FragColor = vec4(vec3(fractalNoise2D(4, v_TexCoord * 10.0)), 1.0);
}