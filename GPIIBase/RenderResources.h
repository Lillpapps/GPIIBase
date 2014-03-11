// RenderSystem_Prereq.h

#pragma once

#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }
/*
class Mesh {
public:
	Mesh();
	~Mesh();

	bool Initialize(ID3D11Device* device);
	void Cleanup();

	ID3D11Buffer *m_vertex_buffer;
	unsigned int m_stride;
	unsigned int m_offset;
	unsigned int m_count;

	ID3D11Buffer* m_index_buffer;
};

class Shader {
public:
	Shader();
	~Shader();

	bool Initialize(ID3D11Device* device, const wchar_t* vshader_filename, const wchar_t* pshader_filename);
	void Cleanup();

	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_input_layout;
};

class ConstantBuffer {
public:
	ConstantBuffer();
	~ConstantBuffer();

	bool Initialize(ID3D11Device* device, unsigned int size, void* buffer);
	void Cleanup();

	void Update(ID3D11DeviceContext* context, unsigned int size, void* buffer);

	ID3D11Buffer* m_buffer;
};

class Texture {
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, const char *filename);
	void Cleanup();

	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_shader_resource_view;
};

class Sampler {
public:
	enum EAddressMode { ADDRESS_WRAP, ADDRESS_MIRROR, ADDRESS_CLAMP };
	enum EFilterMode { FILTER_NEAREST, FILTER_LINEAR };

	Sampler();
	~Sampler();

	bool Initialize(ID3D11Device* device, EAddressMode addr_u, EAddressMode addr_v, EFilterMode filter);
	void Cleanup();

	ID3D11SamplerState* m_sampler;
};

class DepthState {
public:
	enum ECompFunc {
		COMPFUNC_NEVER,
		COMPFUNC_LESS,
		COMPFUNC_EQUAL,
		COMPFUNC_LEQUAL,
		COMPFUNC_GREATER,
		COMPFUNC_NOT_EQUAL,
		COMPFUNC_GREATER_EQUAL,
		COMPFUNC_ALWAYS
	};

	DepthState();
	~DepthState();

	bool Initialize(ID3D11Device* device, bool read, bool write, ECompFunc func);
	void Cleanup();

	ID3D11DepthStencilState* m_depth_state;
};

class RasterizerState {
public:
	enum ECullMode { CULL_NONE, CULL_BACK, CULL_FRONT };
	enum EFillMode { FILL_SOLID, FILL_WIREFRAME };

	RasterizerState();
	~RasterizerState();

	bool Initialize(ID3D11Device* device, ECullMode cull, EFillMode fill, bool clockwise = true);
	void Cleanup();

	ID3D11RasterizerState* m_rasterizer_state;
};


class BlendState {
public:
	enum EBlendOp   {
		BLENDOP_ADD, 
		BLENDOP_SUBTRACT, 
		BLENDOP_REVERSE_SUBTRACT, 
		BLENDOP_MIN, 
		BLENDOP_MAX
	};

	enum EBlendMode {
		BLEND_ZERO, 
		BLEND_ONE, 
		BLEND_SRC_COLOR, 
		BLEND_ONE_MINUS_SRC_COLOR, 
		BLEND_DST_COLOR, 
		BLEND_ONE_MINUS_DST_COLOR,
		BLEND_SRC_ALPHA, 
		BLEND_ONE_MINUS_SRC_ALPHA, 
		BLEND_DST_ALPHA, 
		BLEND_ONE_MINUS_DST_ALPHA, 
		BLEND_SRC_ALPHA_SATURATE
	};

	BlendState();
	~BlendState();

	bool Initialize(ID3D11Device* device, bool enable, EBlendOp blend_rgb, EBlendMode src_color, EBlendMode dst_color, 
		EBlendOp blend_alpha, EBlendMode src_alpha, EBlendMode dst_alpha);
	void Cleanup();

	ID3D11BlendState* m_blend_state;
};
*/