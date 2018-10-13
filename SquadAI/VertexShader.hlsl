struct Input
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Output main(Input input)
{
	Output output;

	output.position = float4(input.position.x, input.position.y, input.position.z, 1);
	output.color = input.color;

	return output;
}