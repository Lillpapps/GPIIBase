// RenderSystem_PreReq.cpp

#include "stdafx.h"
#include "RenderResources.h"

/*
Mesh::Mesh() {
	m_vertex_buffer = nullptr;
	m_stride = 0;
	m_offset = 0;
	m_count = 0;
	m_index_buffer = 0;
};

Mesh::~Mesh() {
	Cleanup();
};

bool Mesh::Initialize(ID3D11Device* device) {
	struct Vec2 {
		float x, y;
	};
	struct Vertex {
		Vec3 p;
		Vec2 t;
		Vec3 n;
	};

	float v = 1.0f;
	// 512 x 512 
	float s[5] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
	float t[3] = { 0.0f, 0.5f, 1.0f};

	Vertex vertices[] = {
		// front
		{ Vec3(-v, v,-v), { 0.0f, 0.0f }, Vec3( 0.0f, 0.0f, -1.0f ) }, // 0, 1, 2, 2, 3, 0,
		{ Vec3( v, v,-v), { 1.0f, 0.0f }, Vec3( 0.0f, 0.0f, -1.0f ) },
		{ Vec3( v,-v,-v), { 1.0f, 1.0f }, Vec3( 0.0f, 0.0f, -1.0f ) },
		{ Vec3(-v,-v,-v), { 0.0f, 1.0f }, Vec3( 0.0f, 0.0f, -1.0f ) },

		// right
		{ Vec3( v, v,-v), { s[1], t[1] }, Vec3( 1.0f, 0.0f, 0.0f ) }, // 4, 5, 6, 6, 7, 4
		{ Vec3( v, v, v), { s[2], t[1] }, Vec3( 1.0f, 0.0f, 0.0f ) },
		{ Vec3( v,-v, v), { s[2], t[2] }, Vec3( 1.0f, 0.0f, 0.0f ) },
		{ Vec3( v,-v,-v), { s[1], t[2] }, Vec3( 1.0f, 0.0f, 0.0f ) },

		// back
		{ Vec3( v, v, v), { s[2], t[1] }, Vec3( 0.0f, 0.0f, 1.0f ) }, // 8, 9, 10, 10, 11, 8
		{ Vec3(-v, v, v), { s[3], t[1] }, Vec3( 0.0f, 0.0f, 1.0f ) },
		{ Vec3(-v,-v, v), { s[3], t[2] }, Vec3( 0.0f, 0.0f, 1.0f ) },
		{ Vec3( v,-v, v), { s[2], t[2] }, Vec3( 0.0f, 0.0f, 1.0f ) },

		// left
		{ Vec3(-v, v, v), { s[3], t[1] }, Vec3(-1.0f, 0.0f, 0.0f ) }, // 12, 13, 15, 15, 16, 12
		{ Vec3(-v, v,-v), { s[4], t[1] }, Vec3(-1.0f, 0.0f, 0.0f ) },
		{ Vec3(-v,-v,-v), { s[4], t[2] }, Vec3(-1.0f, 0.0f, 0.0f ) },		
		{ Vec3(-v,-v, v), { s[3], t[2] }, Vec3(-1.0f, 0.0f, 0.0f ) },

		// top
		{ Vec3(-v, v, v), { s[2], t[0] }, Vec3( 0.0f, 1.0f, 0.0f ) }, // 16, 17, 18, 18, 19, 16
		{ Vec3( v, v, v), { s[2], t[1] }, Vec3( 0.0f, 1.0f, 0.0f ) },
		{ Vec3( v, v,-v), { s[1], t[1] }, Vec3( 0.0f, 1.0f, 0.0f ) },
		{ Vec3(-v, v,-v), { s[1], t[0] }, Vec3( 0.0f, 1.0f, 0.0f ) },

		// bottom
		{ Vec3(-v,-v,-v), { 0.0f, 1.0f }, Vec3( 0.0f,-1.0f, 0.0f ) }, // 20, 21, 22, 22, 23, 20
		{ Vec3( v,-v,-v), { 0.0f, 1.0f }, Vec3( 0.0f,-1.0f, 0.0f ) },
		{ Vec3( v,-v, v), { 0.0f, 1.0f }, Vec3( 0.0f,-1.0f, 0.0f ) },
		{ Vec3(-v,-v, v), { 0.0f, 1.0f }, Vec3( 0.0f,-1.0f, 0.0f ) },
	};

	m_stride = sizeof(Vertex);

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(vertices);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_vertex_buffer);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create vertex buffer!", "VertexBufferError!", MB_OK|MB_ICONERROR);
		return false;
	};

	short indices[] = {
		0,1,2, 2,3,0,
		4,5,6, 6,7,4,
		8,9,10, 10,11,8,
		12,13,14, 14,15,12,
		16,17,18, 18,19,16,
		20,21,22, 22,23,20
	};

	m_count = 6;//sizeof(indices) / sizeof(indices[0]);

	desc.ByteWidth = sizeof(indices);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	data.pSysMem = indices;

	hr = device->CreateBuffer(&desc, &data, &m_index_buffer);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create index buffer!", "IndexBufferError!", MB_OK|MB_ICONERROR);
		return false;
	};

	return true;
};

void Mesh::Cleanup() {
	SAFE_RELEASE(m_vertex_buffer);
};

Shader::Shader() {
	m_vertex_shader = nullptr;
	m_pixel_shader = nullptr;
	m_input_layout = nullptr;
};

Shader::~Shader() {
	Cleanup();
};

bool Shader::Initialize(ID3D11Device* device, const wchar_t* vshader_filename, const wchar_t* pshader_filename) {
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		vshader_filename, 
		NULL, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		"main", 
		"vs_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
		0,
		&shader, 
		&error);
	if(hr != S_OK) {
		MessageBoxA(NULL, (LPCSTR)error->GetBufferPointer(), "ShaderError!", MB_OK|MB_ICONERROR);
		error->Release();
		return false;
	};

	hr = device->CreateVertexShader(shader->GetBufferPointer(), 
		shader->GetBufferSize(), 
		NULL,
		&m_vertex_shader);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create vertex shader!", "ShaderError!", MB_OK|MB_ICONERROR);
		return false;
	};

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = device->CreateInputLayout(input_desc, 
		sizeof(input_desc) / sizeof(input_desc[0]),
		shader->GetBufferPointer(), shader->GetBufferSize(),
		&m_input_layout);
	
	shader->Release();
	shader = nullptr;

	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create input layout!", "InputLayoutError!", MB_OK|MB_ICONERROR);
		return false;
	};

	hr = D3DCompileFromFile(
		pshader_filename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
		0,
		&shader, 
		&error);
	if(hr != S_OK) {
		MessageBoxA(NULL, (LPCSTR)error->GetBufferPointer(), "ShaderError!", MB_OK|MB_ICONERROR);
		error->Release();
		return false;
	};

	hr = device->CreatePixelShader(shader->GetBufferPointer(),
		shader->GetBufferSize(),
		NULL,
		&m_pixel_shader);
	shader->Release();

	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create pixel shader!", "ShaderError!", MB_OK|MB_ICONERROR);
		return false;
	};

	return true;
};

void Shader::Cleanup() {
	SAFE_RELEASE(m_vertex_shader);
	SAFE_RELEASE(m_pixel_shader);
	SAFE_RELEASE(m_input_layout);
};


ConstantBuffer::ConstantBuffer() {
	m_buffer = nullptr;
};

ConstantBuffer::~ConstantBuffer() {
	Cleanup();
};

bool ConstantBuffer::Initialize(ID3D11Device* device, unsigned int size, void* buffer) {
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_buffer);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create constant buffer!", "ConstantBufferError!", MB_OK|MB_ICONERROR);
		return false;
	};
	return true;
};

void ConstantBuffer::Cleanup() {
	SAFE_RELEASE(m_buffer);
};

void ConstantBuffer::Update(ID3D11DeviceContext* context, unsigned int size, void* buffer) {
	context->UpdateSubresource(m_buffer, 0, NULL, buffer, 0, 0);
};


Texture::Texture() {
	m_texture = nullptr;
	m_shader_resource_view = nullptr;
};

Texture::~Texture() {
	Cleanup();
};

bool Texture::Initialize(ID3D11Device* device, const char *filename) {
	FILE* file = fopen(filename, "rb");
	if(!file) return false;

	int width = 0, height = 0, components = 0;
	unsigned char* buffer = stbi_load_from_file(file, &width, &height, &components, 0);
	fclose(file);

	const DXGI_FORMAT formats[] = { 
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8G8B8A8_UNORM,
	};

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = formats[components];
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = width * components;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &m_texture);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create texture 2D!", "TextureError!", MB_OK|MB_ICONERROR);
		stbi_image_free(buffer);
		return false;
	};

	stbi_image_free(buffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_resource_view);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create shader resource view for texture 2D!", "ShaderResourceViewError!", MB_OK|MB_ICONERROR);
		return false;
	};


	return true;
};

void Texture::Cleanup() {
	SAFE_RELEASE(m_texture);
};


Sampler::Sampler() {
	m_sampler = nullptr;
};

Sampler::~Sampler() {
	Cleanup();
};

bool Sampler::Initialize(ID3D11Device* device, EAddressMode addr_u, EAddressMode addr_v, EFilterMode filter) {
	const D3D11_TEXTURE_ADDRESS_MODE address[] = { D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_MIRROR, D3D11_TEXTURE_ADDRESS_CLAMP };
	const D3D11_FILTER filters[] = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR };

	D3D11_SAMPLER_DESC desc;
	desc.Filter = filters[filter];

	desc.AddressU = address[addr_u];
	desc.AddressV = address[addr_v];
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = FLT_MAX;

	HRESULT hr = device->CreateSamplerState(&desc, &m_sampler);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create sampler state!", "SamplerError!", MB_OK|MB_ICONERROR);
		return false;
	};

	return true;
};

void Sampler::Cleanup() {
	SAFE_RELEASE(m_sampler);
};


DepthState::DepthState() {
	m_depth_state = nullptr;
};

DepthState::~DepthState() {
	Cleanup();
};

bool DepthState::Initialize(ID3D11Device* device, bool read, bool write, ECompFunc func) {
	const D3D11_COMPARISON_FUNC funcs[] = {
		D3D11_COMPARISON_NEVER,
		D3D11_COMPARISON_LESS,
		D3D11_COMPARISON_EQUAL,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_GREATER,
		D3D11_COMPARISON_NOT_EQUAL,
		D3D11_COMPARISON_GREATER_EQUAL,
		D3D11_COMPARISON_ALWAYS,
	};

	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = read ? TRUE : FALSE;
	desc.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = funcs[func];
	desc.StencilEnable = FALSE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	HRESULT hr = device->CreateDepthStencilState(&desc, &m_depth_state);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create depth stencil state!", "DepthStencilStateError!", MB_OK|MB_ICONERROR);
		return false;
	};
	return true;
};

void DepthState::Cleanup() {
	SAFE_RELEASE(m_depth_state);
};


RasterizerState::RasterizerState() {
	m_rasterizer_state = nullptr;
};

RasterizerState::~RasterizerState() {
	Cleanup();
};

bool RasterizerState::Initialize(ID3D11Device* device, ECullMode cull, EFillMode fill, bool clockwise) {
	const D3D11_CULL_MODE cullmodes[] = { D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_CULL_FRONT };
	const D3D11_FILL_MODE fillmodes[] = { D3D11_FILL_SOLID, D3D11_FILL_WIREFRAME };

	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = fillmodes[fill];
	desc.CullMode = cullmodes[cull];
	desc.FrontCounterClockwise = clockwise ? FALSE : TRUE;
	desc.DepthBias = 0;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	HRESULT hr = device->CreateRasterizerState(&desc, &m_rasterizer_state);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create rasterizer state!", "RasterizerStateError!", MB_OK|MB_ICONERROR);
		return false;
	};

	return true;
};

void RasterizerState::Cleanup() {
	SAFE_RELEASE(m_rasterizer_state);
};

BlendState::BlendState() {
	m_blend_state = nullptr;
};

BlendState::~BlendState() {
	Cleanup();
};

bool BlendState::Initialize(ID3D11Device* device, bool enable, 
							EBlendOp blend_rgb, EBlendMode src_color, EBlendMode dst_color, 
							EBlendOp blend_alpha, EBlendMode src_alpha, EBlendMode dst_alpha) 
{
	const D3D11_BLEND_OP ops[] = {
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_MIN,
		D3D11_BLEND_OP_MAX,
	};

	const D3D11_BLEND modes[] = {
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_COLOR,
		D3D11_BLEND_INV_SRC_COLOR,
		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_INV_DEST_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_INV_DEST_ALPHA,
		D3D11_BLEND_SRC_ALPHA_SAT,
	};

	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[0].BlendEnable = enable ? TRUE : FALSE;
	desc.RenderTarget[0].BlendOp = ops[blend_rgb];
	desc.RenderTarget[0].BlendOpAlpha = ops[blend_alpha];
	desc.RenderTarget[0].SrcBlend = modes[src_color];
	desc.RenderTarget[0].DestBlend = modes[dst_color];
	desc.RenderTarget[0].SrcBlendAlpha = modes[src_alpha];
	desc.RenderTarget[0].DestBlendAlpha = modes[dst_alpha];
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = device->CreateBlendState(&desc, &m_blend_state);
	if(hr != S_OK) {
		MessageBoxA(NULL, "Could not create blend state!", "BlendStateError!", MB_OK|MB_ICONERROR);
		return false;
	};

	return true;
};

void BlendState::Cleanup() {
	SAFE_RELEASE(m_blend_state);
};
*/