// RenderSystem.cpp

#include "stdafx.h"
#include "RenderSystem.h"

#if defined(NDEBUG)
#define ASSERT(x)
#else
#include <cassert>
#define ASSERT(x) assert(x)
#endif

#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }

struct Constant {
	std::string name;
	char* vs_data;
	char* ps_data;
	int vs_buffer;
	int ps_buffer;
};

struct Sampler {
	std::string name;
	int vs_index;
	int ps_index;
};

struct RenderSystem::Shader {
	ID3D11VertexShader* vertex;
	ID3D11PixelShader* pixel;
	ID3DBlob* signature;
	int num_vs_constants;
	ID3D11Buffer** vs_constants;
	int num_ps_constants;
	ID3D11Buffer** ps_constants;
	int num_constants;
	Constant *constants;
	int num_textures;
	Sampler* textures;
	int num_samplers;
	Sampler* samplers;
	char** vs_mem;
	char** ps_mem;
	bool* vs_dirty;
	bool* ps_dirty;
};

struct RenderSystem::Texture {
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	DXGI_FORMAT format;
	int width, height, components;
};

struct RenderSystem::VertexBuffer {
	ID3D11Buffer* vertex_buffer;
	unsigned int vertex_size;
};

struct RenderSystem::IndexBuffer {
	ID3D11Buffer* index_buffer;
	unsigned int index_size;
	unsigned int index_count;
};

struct RenderSystem::VertexFormat {
	ID3D11InputLayout* input_layout;
	unsigned int vertex_size;
};

struct RenderSystem::SamplerState {
	ID3D11SamplerState* state;
};

struct RenderSystem::BlendState {
	ID3D11BlendState* state;
};

struct RenderSystem::DepthState {
	ID3D11DepthStencilState* state;
};

struct RenderSystem::RasterizerState {
	ID3D11RasterizerState* state;
};

#if !defined(NDEBUG)
static ID3D11Debug* debug;
#endif

RenderSystem::RenderSystem(HWND hWnd, int width, int height) {
	m_hWnd = hWnd;
	m_width = width;
	m_height = height;

	m_swap_chain = nullptr;
	m_device = nullptr;
	m_context = nullptr;
	m_render_target_view = nullptr;
	m_depth_stencil_buffer = nullptr;
	m_depth_stencil_view = nullptr;

	DXGI_MODE_DESC mode = { 0 };
	mode.Width = m_width;
	mode.Height = m_height;
	mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mode.RefreshRate.Numerator = 60;
	mode.RefreshRate.Denominator = 1;

	DXGI_SAMPLE_DESC sample = { 0 };
	sample.Count = 1;
	sample.Quality = 0;

	DXGI_SWAP_CHAIN_DESC desc = { 0 };
	desc.BufferCount = 1;
	desc.BufferDesc = mode;
	desc.SampleDesc = sample;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Windowed = TRUE;
	desc.OutputWindow = m_hWnd;

	unsigned int      num = 1;
	D3D_FEATURE_LEVEL req = D3D_FEATURE_LEVEL_11_0, sup;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, 
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL,
		D3D11_CREATE_DEVICE_DEBUG, 
		&req, 
		num,
		D3D11_SDK_VERSION,
		&desc,
		&m_swap_chain, 
		&m_device,
		&sup, 
		&m_context);
	if(hr != S_OK) {
		throw std::exception("Could not device and swap chain!");
	};

	ID3D11Texture2D* back_buffer = nullptr;
	hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
	if(hr != S_OK) {
		throw std::exception("Could not access back buffer of swap chain!");
		return;
	};
	back_buffer->Release();

	m_render_target_view = nullptr;
	m_device->CreateRenderTargetView(back_buffer, NULL, &m_render_target_view);

	// create depth buffer
	D3D11_TEXTURE2D_DESC dsb_desc;
	dsb_desc.Width = m_width;
	dsb_desc.Height = m_height;
	dsb_desc.MipLevels = 1;
	dsb_desc.ArraySize = 1;
	dsb_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsb_desc.SampleDesc.Count = 1;
	dsb_desc.SampleDesc.Quality = 0;
	dsb_desc.Usage = D3D11_USAGE_DEFAULT;
	dsb_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsb_desc.CPUAccessFlags = 0;
	dsb_desc.MiscFlags = 0;

	hr = m_device->CreateTexture2D(&dsb_desc, NULL, &m_depth_stencil_buffer);
	if(hr != S_OK) {
		throw std::exception("Could not create depth stencil buffer!");
	};

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = 0;
	dsv_desc.Format = dsb_desc.Format;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	hr = m_device->CreateDepthStencilView(m_depth_stencil_buffer, &dsv_desc, &m_depth_stencil_view);
	if(hr != S_OK) {
		throw std::exception("Could not create depth stencil view!");
	};

	m_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_width;
	viewport.Height = (float)m_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &viewport);

	for(unsigned int i = 0; i < 10; i++) {
		m_selected.S[i] = INVALID_RESOURCE;
		m_current.S[i] = INVALID_RESOURCE;
	};

#if !defined(NDEBUG)
	m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
#endif
};

RenderSystem::~RenderSystem() {
	/*
		Ignore DXGI / D3D11 warnings in output window: 
		-http://blogs.msdn.com/b/nativeconcurrency/archive/2013/02/19/harmless-dxgi-warnings-with-c-amp.aspx
	*/
	for(unsigned int i = 0; i < m_rasterizer_states.size(); i++) {
		RasterizerState &state = m_rasterizer_states[i];
		if(!state.state) continue;
		state.state->Release();
		state.state = nullptr;
	};
	m_rasterizer_states.clear();

	for(unsigned int i = 0; i < m_depth_states.size(); i++) {
		DepthState &state = m_depth_states[i];
		if(!state.state) continue;
		state.state->Release();
		state.state = nullptr;
	};
	m_depth_states.clear();

	for(unsigned int i = 0; i < m_blend_states.size(); i++) {
		BlendState &state = m_blend_states[i];
		if(!state.state) continue;
		state.state->Release();
		state.state = nullptr;
	};
	m_blend_states.clear();

	for(unsigned int i = 0; i < m_sampler_states.size(); i++) {
		SamplerState &state = m_sampler_states[i];
		if(!state.state) continue;
		state.state->Release();
		state.state = nullptr;
	};
	m_sampler_states.clear();

	for(unsigned int i = 0; i < m_index_buffers.size(); i++) {
		IndexBuffer &index_buffer = m_index_buffers[i];
		if(!index_buffer.index_buffer) continue;
		index_buffer.index_buffer->Release();
		index_buffer.index_buffer = nullptr;
		index_buffer.index_count = 0;
		index_buffer.index_size = 0;
	};
	m_index_buffers.clear();

	for(unsigned int i = 0; i < m_vertex_buffers.size(); i++) {
		VertexBuffer &vertex_buffer = m_vertex_buffers[i];
		if(!vertex_buffer.vertex_buffer) continue;
		vertex_buffer.vertex_buffer->Release();
		vertex_buffer.vertex_buffer = nullptr;
		vertex_buffer.vertex_size = 0;
	};
	m_vertex_buffers.clear();

	for(unsigned int i = 0; i < m_vertex_formats.size(); i++) {
		VertexFormat &format = m_vertex_formats[i];
		if(!format.input_layout) continue;
		format.input_layout->Release();
		format.input_layout = nullptr;
		format.vertex_size = 0;
	};
	m_vertex_formats.clear();

	for(unsigned int i = 0; i < m_textures.size(); i++) {
		Texture &texture = m_textures[i];
		if(!texture.texture) continue;
		texture.texture->Release();
		texture.texture = nullptr;
		texture.srv->Release();
		texture.srv = nullptr;
		texture.components = 0;
		texture.format = DXGI_FORMAT_UNKNOWN;
		texture.width = 0;
		texture.height = 0;
	};
	m_textures.clear();
	
	for(unsigned int i = 0; i < m_shaders.size(); i++) {
		Shader &shader = m_shaders[i];
		if(!shader.signature) continue;

		shader.vertex->Release();
		shader.vertex = nullptr;
		shader.pixel->Release();
		shader.pixel = nullptr;

		delete[] shader.constants;
		shader.constants = nullptr;

		delete[] shader.textures;
		shader.textures = nullptr;
	
		delete[] shader.samplers;
		shader.samplers = nullptr;

		for(int i = 0; i < shader.num_vs_constants; i++) {
			shader.vs_constants[i]->Release();
			shader.vs_constants[i] = nullptr;
			delete[] shader.vs_mem[i];
			shader.vs_mem[i] = nullptr;
		};
		delete[] shader.vs_constants;
		shader.vs_constants = nullptr;
		delete[] shader.vs_dirty;
		shader.vs_dirty = nullptr;

		for(int i = 0; i < shader.num_ps_constants; i++) {
			shader.ps_constants[i]->Release();
			shader.ps_constants[i] = nullptr;
			delete[] shader.ps_mem[i];
			shader.ps_mem[i] = nullptr;
		};
		delete[] shader.ps_constants;
		shader.ps_constants = nullptr;
		delete[] shader.ps_dirty;
		shader.ps_dirty = nullptr;

		shader.signature->Release();
		shader.signature = nullptr;
	};
	m_shaders.clear();

	SAFE_RELEASE(m_depth_stencil_buffer);
	SAFE_RELEASE(m_depth_stencil_view);
	SAFE_RELEASE(m_render_target_view);

#if !defined(NDEBUG)
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	SAFE_RELEASE(m_context);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_swap_chain);
};

void RenderSystem::Clear() {
	m_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_context->ClearRenderTargetView(m_render_target_view, DirectX::Colors::CornflowerBlue);
};

void RenderSystem::Present() {
	m_swap_chain->Present(0, 0);
};

int RenderSystem::CreateShader(const std::string& vertex_source, const std::string& pixel_source) {
	HRESULT hr = S_OK;
	ID3DBlob *shader_blob = nullptr, *error_blob = nullptr;
	ID3D11ShaderReflection *vs_reflection = nullptr, *ps_reflection = nullptr;

	Shader shader;
	memset(&shader, 0, sizeof(Shader));

	if(D3DCompile(vertex_source.c_str(), vertex_source.length(), NULL, NULL, NULL, "main", "vs_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS 
#if defined(_DEBUG)
		| D3DCOMPILE_DEBUG
#endif
		, 0, &shader_blob, &error_blob) == S_OK) 
	{
		if(m_device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), NULL, &shader.vertex) == S_OK) {
			D3DReflect(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vs_reflection);
			D3DGetInputSignatureBlob(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), &shader.signature);

			/*D3D11_SHADER_DESC sh_desc;
			vs_reflection->GetDesc(&sh_desc);
			std::vector<D3D11_INPUT_ELEMENT_DESC> input_descs;
			unsigned int offset = 0;
			for(unsigned int i = 0; i < sh_desc.InputParameters; i++) {
				D3D11_SIGNATURE_PARAMETER_DESC sig_desc;
				vs_reflection->GetInputParameterDesc(i, &sig_desc);
				if(sig_desc.SystemValueType == D3D_NAME_UNDEFINED && sig_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
					D3D11_INPUT_ELEMENT_DESC desc;
					desc.SemanticName = sig_desc.SemanticName;
					desc.SemanticIndex = sig_desc.SemanticIndex;
					desc.AlignedByteOffset = offset;
					desc.InputSlot = 0;
					desc.InstanceDataStepRate = 0;
					desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					switch(sig_desc.Mask) {
					case 0x1:
						desc.Format = DXGI_FORMAT_R32_FLOAT;
						offset += sizeof(float);
						break;
					case 0x3:
						desc.Format = DXGI_FORMAT_R32G32_FLOAT;
						offset += sizeof(float) * 2;
						break;
					case 0x7:
						desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
						offset += sizeof(float) * 3;
						break;
					case 0xF:
						desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						offset += sizeof(float) * 4;
						break;
					};
				};
			};*/
		};
	}
	else {
		Debug::Msg("ShaderError: %s", (char*)error_blob->GetBufferPointer());
	};
	SAFE_RELEASE(shader_blob);
	SAFE_RELEASE(error_blob);

	if(D3DCompile(pixel_source.c_str(), pixel_source.length(), NULL, NULL, NULL, "main", "ps_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS 
#if defined(_DEBUG)
		| D3DCOMPILE_DEBUG
#endif
		, 0, &shader_blob, &error_blob) == S_OK) 
	{
		if(m_device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), NULL, &shader.pixel) == S_OK) {
			D3DReflect(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&ps_reflection);
		};
	}
	else {
	};
	SAFE_RELEASE(shader_blob);
	SAFE_RELEASE(error_blob);

	D3D11_SHADER_DESC vs_desc, ps_desc;
	if(vs_reflection) {
		vs_reflection->GetDesc(&vs_desc);
		if(vs_desc.ConstantBuffers > 0) {
			shader.num_vs_constants = vs_desc.ConstantBuffers;
			shader.vs_constants = new ID3D11Buffer*[shader.num_vs_constants];
			shader.vs_mem = new char*[shader.num_vs_constants];
			shader.vs_dirty = new bool[shader.num_vs_constants];
		};
	};

	if(ps_reflection) {
		ps_reflection->GetDesc(&ps_desc);
		if(ps_desc.ConstantBuffers > 0) {
			shader.num_ps_constants = ps_desc.ConstantBuffers;
			shader.ps_constants = new ID3D11Buffer*[shader.num_ps_constants];
			shader.ps_mem = new char*[shader.num_ps_constants];
			shader.ps_dirty = new bool[shader.num_ps_constants];
		};
	};

	D3D11_BUFFER_DESC cb_desc;
	cb_desc.Usage = D3D11_USAGE_DEFAULT;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.MiscFlags = 0;

	std::vector<Constant> constants;

	for(int i = 0; i < shader.num_vs_constants; i++) {
		D3D11_SHADER_BUFFER_DESC sb_desc;
		vs_reflection->GetConstantBufferByIndex(i)->GetDesc(&sb_desc);

		cb_desc.ByteWidth = sb_desc.Size;
		m_device->CreateBuffer(&cb_desc, NULL, &shader.vs_constants[i]);
		shader.vs_mem[i] = new char[sb_desc.Size];
		for(unsigned int k = 0; k < sb_desc.Variables; k++) {
			D3D11_SHADER_VARIABLE_DESC var_desc;
			vs_reflection->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&var_desc);

			Constant constant;
			constant.name = var_desc.Name;
			constant.vs_data = shader.vs_mem[i] + var_desc.StartOffset;
			constant.vs_buffer = i;
			constant.ps_data = nullptr;
			constant.ps_buffer = -1;
			constants.push_back(constant);
		};
		shader.vs_dirty[i] = false;
	};

	unsigned int num_constants = constants.size();
	for(int i = 0; i < shader.num_ps_constants; i++) {
		D3D11_SHADER_BUFFER_DESC sb_desc;
		ps_reflection->GetConstantBufferByIndex(i)->GetDesc(&sb_desc);

		cb_desc.ByteWidth = sb_desc.Size;
		m_device->CreateBuffer(&cb_desc, NULL, &shader.ps_constants[i]);
		shader.ps_mem[i] = new char[sb_desc.Size];
		for(unsigned int k = 0; k < sb_desc.Variables; k++) {
			D3D11_SHADER_VARIABLE_DESC var_desc;
			ps_reflection->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&var_desc);

			int index = -1;
			for(unsigned int j = 0; j < num_constants; j++) {
				if(strcmp(constants[j].name.c_str(), var_desc.Name) == 0) {
					index = j;
					break;
				};
			};
			if(index != -1) {
				constants[index].ps_data = shader.ps_mem[i] + var_desc.StartOffset;
				constants[index].ps_buffer = i;
			}
			else {
				Constant constant;
				constant.name = var_desc.Name;
				constant.vs_data = nullptr;
				constant.vs_buffer = -1;
				constant.ps_data = shader.ps_mem[i] + var_desc.StartOffset;
				constant.ps_buffer = i;
				constants.push_back(constant);
			};
		};
	};

	shader.num_constants = constants.size();
	shader.constants = new Constant[shader.num_constants];
	for(int i = 0; i < shader.num_constants; i++) {
		shader.constants[i] = constants[i];
	};
	constants.clear();

	int num_resources = 0;
	num_resources += vs_reflection ? vs_desc.BoundResources : 0;
	num_resources += ps_reflection ? ps_desc.BoundResources : 0;
	if(num_resources > 0) {
		std::vector<Sampler> samplers, textures;

		for(unsigned int i = 0; i < vs_desc.BoundResources; i++) {
			D3D11_SHADER_INPUT_BIND_DESC sib_desc;
			vs_reflection->GetResourceBindingDesc(i, &sib_desc);
			if(sib_desc.Type == D3D_SIT_SAMPLER) {
				Sampler sampler;
				sampler.name = sib_desc.Name;
				sampler.vs_index = sib_desc.BindPoint;
				sampler.ps_index = -1;
				samplers.push_back(sampler);
			}
			else if(sib_desc.Type == D3D_SIT_TEXTURE) {
				Sampler sampler;
				sampler.name = sib_desc.Name;
				sampler.vs_index = sib_desc.BindPoint;
				sampler.ps_index = -1;
				textures.push_back(sampler);
			};
		};

		unsigned int max_textures = textures.size();
		unsigned int max_samplers = samplers.size();
		for(unsigned int i = 0; i < ps_desc.BoundResources; i++) {
			D3D11_SHADER_INPUT_BIND_DESC sib_desc;
			ps_reflection->GetResourceBindingDesc(i, &sib_desc);
			if(sib_desc.Type == D3D_SIT_SAMPLER) {
				int index = -1;
				for(unsigned int k = 0; k < max_samplers; k++) {
					if(strcmp(samplers[k].name.c_str(), sib_desc.Name) == 0) {
						index = k;
						break;
					};
				};
				if(index != -1) {
					samplers[index].ps_index = sib_desc.BindPoint;
				}
				else {
					Sampler sampler;
					sampler.name = sib_desc.Name;
					sampler.vs_index = -1;
					sampler.ps_index = sib_desc.BindPoint;
					samplers.push_back(sampler);
				};
			}
			else if(sib_desc.Type == D3D_SIT_TEXTURE) {
				int index = -1;
				for(unsigned int k = 0; k < max_textures; k++) {
					if(strcmp(textures[k].name.c_str(), sib_desc.Name) == 0) {
						index = k;
						break;
					};
				};
				if(index != -1) {
					textures[index].ps_index = sib_desc.BindPoint;
				}
				else {
					Sampler sampler;
					sampler.name = sib_desc.Name;
					sampler.vs_index = -1;
					sampler.ps_index = sib_desc.BindPoint;
					textures.push_back(sampler);
				};
			};
		};

		shader.num_textures = textures.size();
		shader.textures = new Sampler[shader.num_textures];
		for(unsigned int i = 0; i < textures.size(); i++) {
			shader.textures[i] = textures[i];
		};
		textures.clear();

		shader.num_samplers = samplers.size();
		shader.samplers = new Sampler[shader.num_samplers];
		for(unsigned int i = 0; i < samplers.size(); i++) {
			shader.samplers[i] = samplers[i];
		};
		samplers.clear();
	};

	if(vs_reflection) 
		vs_reflection->Release();
	if(ps_reflection) 
		ps_reflection->Release();

	m_shaders.push_back(shader);
	return (int)(m_shaders.size() - 1);
};

void RenderSystem::DestroyShader(int id) {
	ASSERT(id != INVALID_RESOURCE);
	
	Shader &shader = m_shaders[id];
	shader.vertex->Release();
	shader.vertex = nullptr;
	shader.pixel->Release();
	shader.pixel = nullptr;

	delete[] shader.constants;
	shader.constants = nullptr;

	delete[] shader.textures;
	shader.textures = nullptr;
	
	delete[] shader.samplers;
	shader.samplers = nullptr;

	for(int i = 0; i < shader.num_vs_constants; i++) {
		shader.vs_constants[i]->Release();
		shader.vs_constants[i] = nullptr;
		delete[] shader.vs_mem[i];
		shader.vs_mem[i] = nullptr;
	};
	delete[] shader.vs_constants;
	shader.vs_constants = nullptr;
	delete[] shader.vs_dirty;
	shader.vs_dirty = nullptr;

	for(int i = 0; i < shader.num_ps_constants; i++) {
		shader.ps_constants[i]->Release();
		shader.ps_constants[i] = nullptr;
		delete[] shader.ps_mem[i];
		shader.ps_mem[i] = nullptr;
	};
	delete[] shader.ps_constants;
	shader.ps_constants = nullptr;
	delete[] shader.ps_dirty;
	shader.ps_dirty = nullptr;

	shader.signature->Release();
	shader.signature = nullptr;
};

int RenderSystem::CreateTexture(void* buffer, unsigned int width, unsigned int height, unsigned int format) {
	const DXGI_FORMAT formats[] = {
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8G8B8A8_UNORM,
	};
	const int sizes[] = { 1,2,3,4 };

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.Format = formats[format];
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = width * sizes[format];
	data.SysMemSlicePitch = 0;

	Texture texture;

	HRESULT hr = m_device->CreateTexture2D(&desc, &data, &texture.texture);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;
	hr = m_device->CreateShaderResourceView(texture.texture, &srv_desc, &texture.srv);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	texture.format = desc.Format;
	texture.width = width;
	texture.height = height;
	texture.components = sizes[format];
	m_textures.push_back(texture);
	return (int)(m_textures.size() - 1);
};

void RenderSystem::DestroyTexture(int id) {
	ASSERT(id != INVALID_RESOURCE);
	
	Texture &texture = m_textures[id];
	texture.texture->Release();
	texture.texture = nullptr;
	texture.srv->Release();
	texture.srv = nullptr;
	texture.components = 0;
	texture.format = DXGI_FORMAT_UNKNOWN;
	texture.width = 0;
	texture.height = 0;
};

int RenderSystem::CreateVertexFormat(FormatDesc* desc, unsigned int count, int shader_id) {
	ASSERT(shader_id != INVALID_RESOURCE);

	const DXGI_FORMAT formats[][4] = {
		{DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT },
		{DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM }
	};
	const int sizes[] = { sizeof(float), sizeof(short), sizeof(char) };
	const char* semantics[] = { "POSITION", "TEXCOORD", "NORMAL", "COLOR" };

	VertexFormat format;
	format.input_layout = nullptr;
	format.vertex_size = 0;

	int index[4] = { 0 };
	D3D11_INPUT_ELEMENT_DESC il_desc[4];
	for(unsigned int i = 0; i < count; i++) {
		il_desc[i].Format = formats[ desc[i].format ][ desc[i].count - 1];
		il_desc[i].AlignedByteOffset = format.vertex_size;
		il_desc[i].SemanticName = semantics[ desc[i].type ];
		il_desc[i].SemanticIndex = index[ desc[i].type ]++;
		il_desc[i].InputSlot = desc[i].stream;
		il_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		il_desc[i].InstanceDataStepRate = 0;

		format.vertex_size += desc[i].count * sizes[ desc[i].format ];
	};

	Shader &shader = m_shaders[ shader_id ];
	HRESULT hr = m_device->CreateInputLayout(il_desc, count, shader.signature->GetBufferPointer(), shader.signature->GetBufferSize(), &format.input_layout);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_vertex_formats.push_back(format);
	return (int)(m_vertex_formats.size() - 1);
};

void RenderSystem::DestoryVertexFormat(int id) {
	ASSERT(id != INVALID_RESOURCE);

	VertexFormat &format = m_vertex_formats[id];
	format.input_layout->Release();
	format.input_layout = nullptr;
	format.vertex_size = 0;
};

int RenderSystem::CreateVertexBuffer(EBufferAccess access, void* buffer, unsigned int size) {
	const D3D11_USAGE usage[] = { D3D11_USAGE_IMMUTABLE, D3D11_USAGE_DEFAULT, D3D11_USAGE_DYNAMIC };

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = size;
	desc.CPUAccessFlags = access == ACCESS_STREAM ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = usage[access];

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	VertexBuffer vertex_buffer;
	vertex_buffer.vertex_size = size;
	HRESULT hr = m_device->CreateBuffer(&desc, &data, &vertex_buffer.vertex_buffer);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_vertex_buffers.push_back(vertex_buffer);
	return (int)(m_vertex_buffers.size() - 1);
};

void RenderSystem::DestroyVertexBuffer(int id) {
	ASSERT(id != INVALID_RESOURCE);

	VertexBuffer &vertex_buffer = m_vertex_buffers[id];
	vertex_buffer.vertex_buffer->Release();
	vertex_buffer.vertex_buffer = nullptr;
	vertex_buffer.vertex_size = 0;
};

int RenderSystem::CreateIndexBuffer(void* buffer, unsigned int size, unsigned int count) {
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = size * count;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	IndexBuffer index_buffer;
	index_buffer.index_count = count;
	index_buffer.index_size = size;
	index_buffer.index_buffer = nullptr;
	HRESULT hr = m_device->CreateBuffer(&desc, &data, &index_buffer.index_buffer);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_index_buffers.push_back(index_buffer);
	return (int)(m_index_buffers.size() - 1);
};

void RenderSystem::DestroyIndexBuffer(int id) {
	ASSERT(id != INVALID_RESOURCE);

	IndexBuffer &index_buffer = m_index_buffers[id];
	index_buffer.index_buffer->Release();
	index_buffer.index_buffer = nullptr;
	index_buffer.index_count = 0;
	index_buffer.index_size = 0;
};

int RenderSystem::CreateSamplerState(EAddressMode addr_u, EAddressMode addr_v, EFilterMode filter) {
	const D3D11_TEXTURE_ADDRESS_MODE mode[] = { D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_MIRROR, D3D11_TEXTURE_ADDRESS_CLAMP };
	const D3D11_FILTER filters[] = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR };

	D3D11_SAMPLER_DESC desc;
	desc.AddressU = mode[addr_u];
	desc.AddressV = mode[addr_v];
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = filters[filter];
	desc.MaxAnisotropy = 1;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = 0.0f;
	desc.MipLODBias = 0.0f;

	SamplerState sampler;
	HRESULT hr = m_device->CreateSamplerState(&desc, &sampler.state);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_sampler_states.push_back(sampler);
	return (int)(m_sampler_states.size() - 1);
};

void RenderSystem::DestroySamplerState(int id) {
	ASSERT(id != INVALID_RESOURCE);

	SamplerState &state = m_sampler_states[id];
	state.state->Release();
	state.state = nullptr;
};

int RenderSystem::CreateBlendState(bool enable, EBlendOp blend_color, EBlendMode src_color, EBlendMode dst_color, EBlendOp blend_alpha, EBlendMode src_alpha, EBlendMode dst_alpha, int mask, bool alpha_to) {
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
	desc.RenderTarget[0].BlendOp = ops[blend_color];
	desc.RenderTarget[0].BlendOpAlpha = ops[blend_alpha];
	desc.RenderTarget[0].SrcBlend = modes[src_color];
	desc.RenderTarget[0].DestBlend = modes[dst_color];
	desc.RenderTarget[0].SrcBlendAlpha = modes[src_alpha];
	desc.RenderTarget[0].DestBlendAlpha = modes[dst_alpha];
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendState state;
	HRESULT hr = m_device->CreateBlendState(&desc, &state.state);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_blend_states.push_back(state);
	return (int)(m_blend_states.size() - 1);
};

void RenderSystem::DestroyBlendState(int id) {
	ASSERT(id != INVALID_RESOURCE);

	BlendState &state = m_blend_states[id];
	state.state->Release();
	state.state = nullptr;
};

int RenderSystem::CreateDepthState(bool write, bool test, EDepthMode mode) {
	const D3D11_COMPARISON_FUNC modes[] = {
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
	desc.DepthEnable = test ? TRUE : FALSE;
	desc.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = modes[mode];
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

	DepthState state;
	state.state = nullptr;
	HRESULT hr = m_device->CreateDepthStencilState(&desc, &state.state);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_depth_states.push_back(state);
	return (int)(m_depth_states.size() - 1);
};

void RenderSystem::DestroyDepthState(int id) {
	ASSERT(id != INVALID_RESOURCE);

	DepthState& state = m_depth_states[id];
	state.state->Release();
	state.state = nullptr;
};

int RenderSystem::CreateRasterizerState(ECullMode cull, EFillMode fill) {
	const D3D11_CULL_MODE cullmodes[] = { D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_CULL_FRONT };
	const D3D11_FILL_MODE fillmodes[] = { D3D11_FILL_SOLID, D3D11_FILL_WIREFRAME };

	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = fillmodes[fill];
	desc.CullMode = cullmodes[cull];
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	RasterizerState state;
	state.state = nullptr;
	HRESULT hr = m_device->CreateRasterizerState(&desc, &state.state);
	if(hr != S_OK) {
		return INVALID_RESOURCE;
	};

	m_rasterizer_states.push_back(state);
	return (int)(m_rasterizer_states.size() - 1);
};

void RenderSystem::DestroyRasterizerState(int id) {
	ASSERT(id != INVALID_RESOURCE);

	RasterizerState &state = m_rasterizer_states[id];
	state.state->Release();
	state.state = nullptr;
};

void RenderSystem::SetShaderConstant1f(const char* name, const float constant) {
	SetShaderConstantRaw(name, &constant, sizeof(float));
};

void RenderSystem::SetShaderConstant2f(const char* name, const Vec2& constant) {
	SetShaderConstantRaw(name, &constant, sizeof(Vec2));
};

void RenderSystem::SetShaderConstant3f(const char* name, const Vec3& constant) {
	SetShaderConstantRaw(name, &constant, sizeof(Vec3));
};

void RenderSystem::SetShaderConstant4f(const char* name, const Vec3& constant) {
	SetShaderConstantRaw(name, &constant, sizeof(Vec4));
};

void RenderSystem::SetShaderConstantMatrix(const char* name, const Mat4& constant) {
	SetShaderConstantRaw(name, &constant, sizeof(Mat4));
};

void RenderSystem::SetShaderConstantRaw(const char* name, const void* data, unsigned int size) {
	ASSERT(m_selected.m_shader != INVALID_RESOURCE);
	Shader& shader = m_shaders[ m_selected.m_shader ];
	ASSERT(shader.vertex);

	for(int i= 0; i < shader.num_constants; i++) {
		if(strcmp(shader.constants[i].name.c_str(), name) == 0) {
			if(shader.constants[i].vs_data) {
				if(memcmp(shader.constants[i].vs_data, data, size) != 0) {
					memcpy(shader.constants[i].vs_data, data, size);
					shader.vs_dirty[ shader.constants[i].vs_buffer ] = true;
				};
			};
			if(shader.constants[i].ps_data) {
				if(memcmp(shader.constants[i].ps_data, data, size) != 0) {
					memcpy(shader.constants[i].ps_data, data, size);
					shader.ps_dirty[ shader.constants[i].ps_buffer ] = true;
				};
			};
			break;
		};
	};
};

void RenderSystem::SelectShader(int id) {
	m_selected.m_shader = id;
};

void RenderSystem::SelectTexture(int id) {
	m_selected.m_texture = id;
};

void RenderSystem::SelectVertexFormat(int id) {
	m_selected.m_vertex_format = id;
};

void RenderSystem::SelectVertexBuffer(int id) {
	m_selected.m_vertex_buffer = id;
};

void RenderSystem::SelectIndexBuffer(int id) {
	m_selected.m_index_buffer = id;
};

void RenderSystem::SelectSamplerState(int id) {
	m_selected.m_sampler_state = id;
};

void RenderSystem::SelectBlendState(int id) {
	m_selected.m_blend_state = id;
};

void RenderSystem::SelectDepthState(int id) {
	m_selected.m_depth_state = id;
};

void RenderSystem::SelectRasterizerState(int id) {
	m_selected.m_rasterizer_state = id;
};

void RenderSystem::Apply() {
	ApplyShader();
	ApplyShaderConstants();
	ApplyTexture();
	ApplyVertexFormat();
	ApplyVertexBuffer();
	ApplyIndexBuffer();
	ApplySamplerState();
	ApplyBlendState();
	ApplyDepthState();
	ApplyRasterizerState();
};

static const D3D11_PRIMITIVE_TOPOLOGY modes[] = {
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
};
void RenderSystem::Draw(EPrimitiveMode mode, unsigned int start, unsigned int count) {
	if(m_current.m_topology != mode) {
		m_current.m_topology = mode;
		m_context->IASetPrimitiveTopology(modes[mode]);
	};
	m_context->Draw(count, start);
};
void RenderSystem::DrawIndexed(EPrimitiveMode mode, unsigned int start, unsigned int count) {
	if(m_current.m_topology != mode) {
		m_current.m_topology = mode;
		m_context->IASetPrimitiveTopology(modes[mode]);
	};
	m_context->DrawIndexed(count, start, 0);
};

void RenderSystem::UpdateVertexBuffer(int id, unsigned int size, void* buffer) {
	ASSERT(id != INVALID_RESOURCE);
	VertexBuffer &vertex_buffer = m_vertex_buffers[id];

	D3D11_MAPPED_SUBRESOURCE map;
	m_context->Map(vertex_buffer.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, buffer, size);
	m_context->Unmap(vertex_buffer.vertex_buffer, 0);
};

void RenderSystem::ApplyShader() {
	if(m_current.m_shader != m_selected.m_shader) {
		m_current.m_shader = m_selected.m_shader;
		if(m_current.m_shader == INVALID_RESOURCE) {
			m_context->VSSetShader(NULL, NULL, 0);
			m_context->PSSetShader(NULL, NULL, 0);
			return;
		};

		Shader& shader = m_shaders[ m_current.m_shader ];
		m_context->VSSetShader(shader.vertex, NULL, 0);
		m_context->PSSetShader(shader.pixel, NULL, 0);
		if(shader.num_vs_constants)
			m_context->VSSetConstantBuffers(0, shader.num_vs_constants, shader.vs_constants);
		if(shader.num_ps_constants) 
			m_context->PSSetConstantBuffers(0, shader.num_ps_constants, shader.ps_constants);
	};
};

void RenderSystem::ApplyShaderConstants() {
	if(m_current.m_shader == INVALID_RESOURCE) { return; };
	Shader& shader = m_shaders[ m_current.m_shader ];
	for(int i = 0; i < shader.num_vs_constants; i++) {
		if(shader.vs_dirty[i]) {
			shader.vs_dirty[i] = false;
			m_context->UpdateSubresource(shader.vs_constants[i], 0, NULL, shader.vs_mem[i], 0, 0);
		};
	};
	for(int i = 0; i < shader.num_ps_constants; i++) {
		if(shader.ps_dirty[i]) {
			shader.ps_dirty[i] = false;
			m_context->UpdateSubresource(shader.ps_constants[i], 0, NULL, shader.ps_mem[i], 0, 0);
		};
	};
};

void RenderSystem::ApplyTexture() {
	if(m_current.m_texture != m_selected.m_texture) {
		m_current.m_texture = m_selected.m_texture;
		if(m_current.m_texture == INVALID_RESOURCE) { return; };
		m_context->PSSetShaderResources(0, 1, &m_textures[ m_current.m_texture ].srv);
	};
};

void RenderSystem::ApplyVertexFormat() {
	if(m_current.m_vertex_format != m_selected.m_vertex_format) {
		m_current.m_vertex_format = m_selected.m_vertex_format;
		if(m_current.m_vertex_format == INVALID_RESOURCE) {
			m_context->IASetInputLayout(NULL);
			return;
		};
		m_context->IASetInputLayout( m_vertex_formats[ m_current.m_vertex_format ].input_layout );
	};
};

void RenderSystem::ApplyVertexBuffer() {
	if(m_current.m_vertex_buffer != m_selected.m_vertex_buffer) {
		m_current.m_vertex_buffer = m_selected.m_vertex_buffer;
		unsigned int stride[1] = { 0 };
		unsigned int offset[1] = { 0 };
		if(m_current.m_vertex_buffer == INVALID_RESOURCE) {
			ID3D11Buffer* none[] = { nullptr };
			m_context->IASetVertexBuffers(0, 1, none, stride, offset);
			return;
		};
		stride[0] = m_vertex_formats[ m_current.m_vertex_format ].vertex_size;
		m_context->IASetVertexBuffers(0, 1, &m_vertex_buffers[ m_current.m_vertex_buffer ].vertex_buffer, stride, offset);
	};
};

void RenderSystem::ApplyIndexBuffer() {
	if(m_current.m_index_buffer != m_selected.m_index_buffer) {
		m_current.m_index_buffer = m_selected.m_index_buffer;
		if(m_current.m_index_buffer == INVALID_RESOURCE) {
			m_context->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
			return;
		};
		IndexBuffer& buffer = m_index_buffers[ m_current.m_index_buffer ];
		DXGI_FORMAT format = buffer.index_size == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		m_context->IASetIndexBuffer(buffer.index_buffer, format, 0);
	};
};

void RenderSystem::ApplySamplerState() {
	if(m_current.m_sampler_state != m_selected.m_sampler_state) {
		m_current.m_sampler_state = m_selected.m_sampler_state;
		if(m_current.m_sampler_state == INVALID_RESOURCE) { return; };
		m_context->PSSetSamplers(0, 1, &m_sampler_states[ m_current.m_sampler_state ].state);
	};
};

void RenderSystem::ApplyBlendState() {
	if(m_current.m_blend_state != m_selected.m_blend_state) {
		m_current.m_blend_state = m_selected.m_blend_state;
		const FLOAT f[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if(m_current.m_blend_state == INVALID_RESOURCE) {
			m_context->OMSetBlendState(NULL, f, 0xFFFFFFFF);
			return;
		};
		m_context->OMSetBlendState(m_blend_states[ m_current.m_blend_state ].state, f, 0xFFFFFFFF);
	};
};
void RenderSystem::ApplyDepthState() {
	if(m_current.m_depth_state != m_selected.m_depth_state) {
		m_current.m_depth_state = m_selected.m_depth_state;
		if(m_current.m_depth_state == INVALID_RESOURCE) {
			m_context->OMSetDepthStencilState(NULL, 0);
			return;
		};
		m_context->OMSetDepthStencilState(m_depth_states[ m_current.m_depth_state ].state, 0);
	};
};

void RenderSystem::ApplyRasterizerState() {
	if(m_current.m_rasterizer_state != m_selected.m_rasterizer_state) {
		m_current.m_rasterizer_state = m_selected.m_rasterizer_state;
		if(m_current.m_rasterizer_state == INVALID_RESOURCE) {
			m_context->RSSetState(NULL);
			return;
		};
		m_context->RSSetState( m_rasterizer_states[ m_current.m_rasterizer_state ].state );
	};
};
