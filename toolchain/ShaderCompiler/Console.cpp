#include "Console.h"
#include <QEvent>
#include <fstream>
#include <sstream>
#include <QXmlStreamWriter>
#include <QtXml/QDomElement>
#include "zlib.h"
#include "shaderc\shaderc.h"
#include "shaderc\shaderc.hpp"
#include "spirv_hlsl.hpp"
#include "spirv_msl.hpp"
enum e_console_result
{
	CR_VOID_CMD,
	CR_PRINT_HELP,
	CR_INVALID_CMD,
	CR_COMMAND_FAIL,
	CR_COMMAND_SUC
};
static const char*
blGenBase64Encoder(unsigned char* _Blob, int _Size)
{
	const char _b64enc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789" "+/";
	unsigned int _idx, _jdx, _mlen, _rpos, _dif;
	unsigned int _frame = 0;
	char _out[5];
	char* _res;
	_mlen = 4 * _Size / 3 + 1;
	if (_Size % 3)
		_mlen += 4;
	_res = (char*)malloc(_mlen);
	if (!_res)
		return NULL;
	_res[_mlen - 1] = '\0';
	_out[4] = '\0';
	for (_idx = 0; _idx < _Size; _idx += 3)
	{
		_dif = (_Size - _idx) / 3 ? 3 : (_Size - _idx) % 3;
		for (_jdx = 0; _jdx < _dif; ++_jdx)
			memcpy(((char*)&_frame) + 2 - _jdx, _Blob + _idx + _jdx, 1);
		for (_jdx = 0; _jdx < _dif + 1; ++_jdx)
		{
			_out[_jdx] = (char)((_frame & 0xFC0000) >> 18);
			_out[_jdx] = _b64enc[_out[_jdx]];
			_frame = _frame << 6;
		}
		if (_dif == 1)
			_out[2] = _out[3] = '=';
		else if (_dif == 2)
			_out[3] = '=';
		_rpos = (_idx / 3) * 4;
		strcpy(_res + _rpos, _out);
	}
	return _res;
}
static int tokenize(const std::string& str2, const std::string& whitespace_, std::vector<std::string>& outtokens_)
{
	std::string str = str2;
	char* ptr = (char*)str.c_str();
	char* token;
	while (nullptr != (token = strtok(ptr, whitespace_.c_str())))
	{
		outtokens_.push_back(token);
		ptr = nullptr;
	}
	return outtokens_.size();
}

static std::string preprocess_shader(const std::string& source_name, shaderc_shader_kind kind, const std::string& source) 
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");

	shaderc::PreprocessedSourceCompilationResult result =
		compiler.PreprocessGlsl(source, kind, source_name.c_str(), options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << result.GetErrorMessage();
		return "";
	}

	return{ result.cbegin(), result.cend() };
}

static std::string compile_file_to_assembly(const std::string& source_name,
	shaderc_shader_kind kind,
	const std::string& source,
	bool optimize = false) {
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");
	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

	shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(
		source, kind, source_name.c_str(), options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << result.GetErrorMessage();
		return "";
	}

	return{ result.cbegin(), result.cend() };
}

// Compiles a shader to a SPIR-V binary. Returns the binary as
// a vector of 32-bit words.
static std::vector<uint32_t> compile_file(const std::string& source_name,
	shaderc_shader_kind kind,
	const std::string& source,
	bool optimize = false) {
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");
	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

	shaderc::SpvCompilationResult module =
		compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << module.GetErrorMessage();
		return std::vector<uint32_t>();
	}

	return{ module.cbegin(), module.cend() };
}

static void trim(std::string& str)
{
	char ch_ = ' ';
	for (int i = 0; i < str.length(); ++i)
	{
		char code = *(str.c_str() + i);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(0, 1);
		else
			break;
	}
	for (int i = str.length(); i >= 0; --i)
	{
		char code = *(str.c_str() + i - 1);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(str.length() - 1, 1);
		else
			break;
	}
}

static const std::string getEXName(const std::string& _Filename)
{
	const char* _tmp = _Filename.c_str();
	unsigned long _len = 0;
	int _i;
	bool _periodfound = false;
	char* _ext;
	while (*_tmp++)
		_len++;
	_ext = (char*)_Filename.c_str();
	_ext += _len;
	for (_i = _len; _i >= 0; _i--)
	{
		if (*_ext == '.')
		{
			_periodfound = true;
			break;
		}
		_ext--;
	}
	if (!_periodfound)
		return "";
	return  std::string(_ext + 1);
}


static const std::string getVKName(const std::string& _Filename)
{
	std::string xx = _Filename.substr(0, _Filename.size() - 4);
	xx += "bsl";
	return xx;
}

void Console::run()
{
	char str[4096] = { 0 };
	printHelp();
	while (true)
	{
		memset(str, 0, sizeof(str));
		fgets(str, 4096, stdin);
		if (processCmd(str))
			break;
	}
	emit finished();
}

void Console::printHelp()
{
	printf("this is a shader compile tool, you can build bulllord shader from GLSL\n");
	printf("All uniform objects must be in layout(std140)uniform UBO struct\n");
	printf("GLSL shader entry-point must be \"main\"\n");
	printf("shader ext names are .vert .frag .comp .geom .tesc .tese\n");
	printf("input -exec build $shader1$ $shader2$...\n");
	printf("eg : -exec build 2D.vert 2D.frag 2D.gemo\n");
	printf("input -help will show the help information\n");
	printf("input -exit will exit ShaderCompiler\n");
	printf("\n");
}


bool Console::processCmd(const char* cmd)
{
	e_console_result cr;
	std::string str_ = cmd;
	if (str_.length() == 1)
		cr = CR_VOID_CMD;
	std::vector<std::string> args;
	tokenize(str_, " ", args);
	for (int i = 0; i < args.size(); ++i)
		trim(args[i]);
	if (args[0] == "-exec")
	{
		cr = _exec_command(args) ? CR_COMMAND_SUC : CR_COMMAND_FAIL;
		return false;
	}
	else if (args[0] == "-help")
	{
		printHelp();
		cr = CR_PRINT_HELP;
		return false;
	}
	else if (args[0] == "-exit")
		return true;
	else
	{
		error("invalid command");
		cr = CR_INVALID_CMD;
		return false;
	}
}

void Console::print_info(const std::string& context_)
{
	printf(">INFO:%s\n", context_.c_str());
}

void Console::error(const std::string& context_)
{
	printf(">ERROR: %s\n", context_.c_str());
}

bool Console::_exec_command(const std::vector<std::string>& args_)
{
	if (args_[1] == "build")
	{
		int filenum = args_.size() - 2;
		for (int i = 0; i < filenum; ++i)
		{
			std::string outf;
			std::string filename = args_[i + 2];
			bool ret = compile(filename);
			if (i == 0)
				vkname = getVKName(filename);
			if (ret)
			{
				std::stringstream ss;
				ss << "shader " << filename << " compiled successfully";
				print_info(ss.str());
			}
			else
			{
				std::stringstream ss;
				ss << "shader " << filename << " compiled failed";
				error(ss.str());
				return false;
			}
			
		}
		//convert to spec shader(dx, gl, vk, mtl)
		convertMSL();
		convertHLSL();

		//combine to one file(xml)
		combine();
		cleanup();
	}
	return true;
}
bool Console::compile(const std::string& name)
{
	FILE* fp = fopen(name.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* context = (char*)malloc(sz + 1);
	fread(context, 1, sz, fp);
	context[sz] = 0;
	fclose(fp);
	std::string richcontext;
	richcontext = "#version 410 core\n";
	richcontext += context;
	shaderc_shader_kind type = shaderc_glsl_vertex_shader;
	if (getEXName(name) == "vert")
	{
		vert[ST_GL] = context;
		type = shaderc_glsl_vertex_shader;
	}
	else if (getEXName(name) == "frag")
	{
		frag[ST_GL] = context;
		type = shaderc_glsl_fragment_shader;
	}
	else if (getEXName(name) == "geom")
	{
		geom[ST_GL] = context;
		type = shaderc_glsl_geometry_shader;
	}
	else if (getEXName(name) == "tese")
	{
		tese[ST_GL] = context;
		type = shaderc_glsl_tess_evaluation_shader;
	}
	else if (getEXName(name) == "tesc")
	{
		tesc[ST_GL] = context;
		type = shaderc_glsl_tess_control_shader;
	}
	else if (getEXName(name) == "comp")
	{
		comp[ST_GL] = context;
		type = shaderc_glsl_compute_shader;
	}

	std::string preprocessed = preprocess_shader(name, type, richcontext);
	if (!preprocessed.length())
		return false;
	std::cout << "Compiled a vertex shader resulting in preprocessed text:"
		<< std::endl
		<< preprocessed << std::endl;
	std::string assembly = compile_file_to_assembly(name, type,	preprocessed, /* optimize = */ false);
	if (!assembly.length())
		return false;
	std::cout << "Optimized SPIR-V assembly:" << std::endl << assembly << std::endl;
	const std::vector<uint32_t>& spirv = compile_file(name, type, preprocessed, /* optimize = */ false);
	if (!spirv.size())
		return false;
	int spirvsz = spirv.size() * sizeof(uint32_t);
	unsigned char* buf = (unsigned char*)malloc(spirvsz * sizeof(char));
	memcpy(buf, &spirv[0], spirvsz);
	const char* sp64 = blGenBase64Encoder(buf, spirvsz);

	if (getEXName(name) == "vert")
	{
		vert[ST_VK] = sp64;
		vertvb = spirv;
	}
	else if (getEXName(name) == "frag")
	{
		frag[ST_VK] = sp64;
		fragvb = spirv;
	}
	else if (getEXName(name) == "geom")
	{
		geom[ST_VK] = sp64;
		geomvb = spirv;
	}
	else if (getEXName(name) == "tese")
	{
		tese[ST_VK] = sp64;
		tesevb = spirv;
	}
	else if (getEXName(name) == "tesc")
	{
		tesc[ST_VK] = sp64;
		tescvb = spirv;
	}
	else if (getEXName(name) == "comp")
	{
		comp[ST_VK] = sp64;
		compvb = spirv;
	}

	free((char*)sp64);
	free(buf);


	free(context);
	return true;
}

void Console::combine()
{
	//create xml doc
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version=\"1.0\"", "encoding=\"UTF-8\""));
	//~combine xml
	QDomElement BSL = doc.createElement("BSL");
	doc.appendChild(BSL);
	QDomElement GL = doc.createElement("GL");
	//GL
	if (vert[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Vert");		
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(vert[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}

	if (frag[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Frag");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(frag[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}

	if (geom[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Geom");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(geom[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}
	if (tesc[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Tesc");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tesc[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}
	if (tese[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Tese");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tese[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}
	if (comp[ST_GL].length())
	{
		QDomElement vertx = doc.createElement("Comp");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(comp[ST_GL]));
		vertx.appendChild(nodeText);
		GL.appendChild(vertx);
	}
	BSL.appendChild(GL);


	//VK
	QDomElement VK = doc.createElement("VK");
	if (vert[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Vert");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(vert[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}

	if (frag[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Frag");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(frag[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}

	if (geom[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Geom");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(geom[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}
	if (tesc[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Tesc");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tesc[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}
	if (tese[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Tese");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tese[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}
	if (comp[ST_VK].length())
	{
		QDomElement vertx = doc.createElement("Comp");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(comp[ST_VK]));
		vertx.appendChild(nodeText);
		VK.appendChild(vertx);
	}
	BSL.appendChild(VK);


	//DX
	QDomElement DX = doc.createElement("DX");
	if (vert[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Vert");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(vert[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}

	if (frag[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Frag");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(frag[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}

	if (geom[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Geom");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(geom[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}
	if (tesc[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Tesc");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tesc[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}
	if (tese[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Tese");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tese[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}
	if (comp[ST_DX].length())
	{
		QDomElement vertx = doc.createElement("Comp");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(comp[ST_DX]));
		vertx.appendChild(nodeText);
		DX.appendChild(vertx);
	}
	BSL.appendChild(DX);

	//MTL
	QDomElement MTL = doc.createElement("MTL");
	if (vert[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Vert");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(vert[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}

	if (frag[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Frag");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(frag[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}

	if (geom[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Geom");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(geom[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}
	if (tesc[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Tesc");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tesc[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}
	if (tese[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Tese");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(tese[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}
	if (comp[ST_MTL].length())
	{
		QDomElement vertx = doc.createElement("Comp");
		QDomText nodeText = doc.createCDATASection(QString::fromStdString(comp[ST_MTL]));
		vertx.appendChild(nodeText);
		MTL.appendChild(vertx);
	}
	BSL.appendChild(MTL);

	//save xml
	QFile file(QString::fromStdString(vkname));
	if (!file.open(QIODevice::WriteOnly))
		return;
	file.write(doc.toByteArray());
	file.close();
}
using namespace spv;
using namespace spirv_cross;
using namespace std;
bool Console::convertMSL()
{
	//metal
	//vert
	if (vertvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(vertvb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		vert[ST_MTL] = compiler->compile();
	}
	if (fragvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(fragvb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		frag[ST_MTL] = compiler->compile();
	}
	if (geomvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(geomvb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		geom[ST_MTL] = compiler->compile();
	}
	if (tesevb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(tesevb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		tese[ST_MTL] = compiler->compile();
	}
	if (tescvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(tescvb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		tesc[ST_MTL] = compiler->compile();
	}
	if (compvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerMSL>(new CompilerMSL(compvb));
		auto *msl_comp = static_cast<CompilerMSL *>(compiler.get());
		auto msl_opts = msl_comp->get_options();
		msl_opts.pad_and_pack_uniform_structs = true;
		msl_comp->set_options(msl_opts);

		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerMSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		comp[ST_MTL] = compiler->compile();
	}

	return true;
}
bool Console::convertHLSL()
{
	//hlsl
	//vert
	if (vertvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(vertvb));
		compiler->set_entry_point("main"); 
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		vert[ST_DX] = compiler->compile();
	}
	if (fragvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(fragvb));
		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		frag[ST_DX] = compiler->compile();
	}
	if (geomvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(geomvb));
		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		geom[ST_DX] = compiler->compile();
	}
	if (tesevb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(tesevb));
		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		tese[ST_DX] = compiler->compile();
	}
	if (tescvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(tescvb));
		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		tesc[ST_DX] = compiler->compile();
	}
	if (compvb.size())
	{
		unique_ptr<CompilerGLSL> compiler = unique_ptr<CompilerHLSL>(new CompilerHLSL(compvb));
		compiler->set_entry_point("main");
		CompilerGLSL::Options opts = compiler->get_options();
		opts.version = 450;
		opts.force_temporary = false;
		opts.vulkan_semantics = true;
		opts.vertex.fixup_clipspace = false;
		opts.cfg_analysis = true;
		compiler->set_options(opts);

		auto *hlsl = static_cast<CompilerHLSL *>(compiler.get());
		auto hlsl_opts = hlsl->get_options();
		hlsl->set_options(hlsl_opts);

		ShaderResources res;
		res = compiler->get_shader_resources();
		comp[ST_DX] = compiler->compile();
	}

	return true;
}
void Console::cleanup()
{
	for (int i = ST_GL; i < ST_NUM; ++i)
	{
		vert[i] = "";
		frag[i] = "";
		geom[i] = "";
		tese[i] = "";
		tesc[i] = "";
		comp[i] = "";
	}
	vertvb.clear();
	fragvb.clear();
	geomvb.clear();
	tesevb.clear();
	tescvb.clear();
	compvb.clear();
}

