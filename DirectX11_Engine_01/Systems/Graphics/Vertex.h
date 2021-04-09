#pragma once

struct Vertex
{
	Vertex()
		: pos(0.0f, 0.0f, 1.0f), texCoord(1.0f, 1.0f) {}
	Vertex(float x, float y, float z, float u, float v)
		: pos(x,y,z), texCoord(u,v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};

