#pragma once

struct Vertex
{
	Vertex()
		: pos(0.0f, 0.0f, 1.0f), color(1.0f, 1.0f, 1.0f) {}
	Vertex(float x, float y, float z, float r, float g, float b)
		: pos(x,y,z), color(r,g,b) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

