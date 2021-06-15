#include "../../stdafx.h"
#include "Model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)
{
    this->device = device;
    this->deviceContext = deviceContext;
    this->texture = texture;
    this->cb_vs_vertexshader = &cb_vs_vertexShader;

	try
	{
		//Initialize Vertex Buffer
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),	//FRONT 왼쪽 아래
			Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f),	//FRONT 왼쪽 위 
			Vertex(0.5f,  0.5f, -0.5f, 1.0f, 0.0f),		//FRONT 오른쪽 위
			Vertex(0.5f, -0.5f, -0.5f, 1.0f, 1.0f),		//FRONT 오른쪽 아래

			Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f),		//BACK 왼쪽 아래
			Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 0.0f),		//BACK 왼쪽 위 
			Vertex(0.5f,  0.5f, 0.5f, 1.0f, 0.0f),		//BACK 오른쪽 위
			Vertex(0.5f, -0.5f, 0.5f, 1.0f, 1.0f),		//BACK 오른쪽 아래
		};

		//Create vertex buffer & load vertex data.
		HRESULT hr;
		{
			hr = this->vertexBuffer.Initialize(this->device, v, ARRAYSIZE(v));
			COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");
		}


		//Initialize Index Buffer
		DWORD indices[] =
		{
			0, 1, 2,	//FRONT
			2, 3, 0,	//FRONT

			4, 7, 6,	//BACK
			4, 6, 5,	//BACK

			3, 2, 6,	//RIGHT
			3, 6, 7,	//RIGHT

			4, 5, 1,	//LEFT
			4, 1, 0,	//LEFT

			1, 5, 6,	//TOP
			1, 6, 2,	//TOP

			0, 3, 7,	//BOTTOM
			0, 7, 4,	//BOTTOM


		};

		//Create index buffer & load index data
		{
			hr = this->indexBuffer.Initialize(this->device, indices, ARRAYSIZE(indices));
			COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
		}
	}
	catch(COMException& exception)
	{
		ErrorLogger::Log(exception);
	}

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);

    this->UpdateWorldMatrix();
    return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
    this->texture = texture;
}

void Model::Draw(const XMMATRIX& viewProjectionMatrix)
{
    //Update Constant buffer with MVP Matrix
    this->cb_vs_vertexshader->data.mat = this->worldMatrix * viewProjectionMatrix; //Caculate WorldMatrix
    this->cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
    this->cb_vs_vertexshader->ApplyChanges();
    this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

    this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set texture
    this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    UINT offset = 0;
    this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
    this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0); //Draw
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

const XMVECTOR& Model::GetPositionVector() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->posVector;
}

const XMFLOAT3& Model::GetPositionFloat3() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->pos;
}

const XMVECTOR& Model::GetRotationVector() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->rotVector;
}

const XMFLOAT3& Model::GetRotationFloat3() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->rot;
}

void Model::SetPosition(const XMVECTOR& pos)
{
	this->posVector = pos;
	XMStoreFloat3(&this->pos, pos);
	this->UpdateWorldMatrix();

}

void Model::SetPosition(const XMFLOAT3& pos)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->SetPosition(v);
}

void Model::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMFLOAT3& pos)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->AdjustPosition(v);
}

void Model::AdjustPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(pos.x + x, pos.y + y, pos.z + z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMFLOAT3& rot)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->SetRotation(v);
}

void Model::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMFLOAT3& rot)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->AdjustRotation(v);
}

void Model::AdjustRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(rot.x + x, rot.y + y, rot.z + z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = (sqrt((double)lookAtPos.x * (double)lookAtPos.x + (double)lookAtPos.z * (double)lookAtPos.z));
		pitch = (float)atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Model::GetForwardVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_forward;
}

const XMVECTOR& Model::GetRightVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_right;
}

const XMVECTOR& Model::GetBackwardVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_backward;
}

const XMVECTOR& Model::GetLeftVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_left;
}

const XMVECTOR& Model::GetUpVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_up;
}

const XMVECTOR& Model::GetDownVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_down;
}
