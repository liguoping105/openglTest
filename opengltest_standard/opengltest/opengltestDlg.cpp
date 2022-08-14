
// opengltestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "opengltest.h"
#include "opengltestDlg.h"
#include "afxdialogex.h"


#include <libyuv/convert.h>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CopengltestDlg 对话框



CopengltestDlg::CopengltestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENGLTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CopengltestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CopengltestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CopengltestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CopengltestDlg 消息处理程序

BOOL CopengltestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CopengltestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CopengltestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CopengltestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//以下为拷贝
VideoEnv videoEnv;
float dataGL[16] = {};

int width_ = 640;
int height_ = 480;
unsigned char* yuvData_ = nullptr;
Color color_ = { 125,125,125 };
HWND hwnd_ = nullptr;
int pad_[4] = { 0, 0 };



void startupWgl(HWND handle) noexcept
{
	HDC hdc = ::GetDC(handle);
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
		PFD_TYPE_RGBA, // The kind of frame buffer. RGBA or palette.
		32, // Color depth of the frame buffer.
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24, // Number of bits for the depth buffer
		8, // Number of bits for the stencil buffer
		0, // Number of Aux buffers in the frame buffer.
		PFD_MAIN_PLANE, 0, 0, 0, 0 };
	auto no = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, no, &pfd);
	// create a rendering context
	auto hglrc = wglCreateContext(hdc);

	// typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int interval);
	// typedef int(WINAPI * PFNWGLGETSWAPINTERVALEXTPROC)(void);

	// PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapInterval");
	// PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapInterval");

	// wglSwapIntervalEXT(1);
	// int vsync_count = wglGetSwapIntervalEXT(); //

	// make it the calling thread's current rendering context
	wglMakeCurrent(hdc, hglrc);
	// ::ReleaseDC(hwnd, hdc);

}
void initGlew() noexcept
{
	static bool isFirst = true;
	if (isFirst) {
		if (glewInit() != GLEW_OK) {
			exit(EXIT_FAILURE);
		}
		isFirst = false;
	}

}
void init(VideoEnv* videoEnv) noexcept {

	const char* const vertexStringVideo = R"vertexShader(
attribute vec4 vertexIn;
attribute vec2 textureIn;
varying vec2 textureOut;
void main(void) {
	gl_Position = vertexIn;
	textureOut = textureIn;
}
)vertexShader";

#if USE_YUV
	const char* const textureStringVideo = R"fragmentShader(
varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void) {
	vec3 yuv;
	vec3 rgb;
	yuv.x = texture2D(tex_y, textureOut).r;
	yuv.y = texture2D(tex_u, textureOut).r - 0.5;
	yuv.z = texture2D(tex_v, textureOut).r - 0.5;
	rgb = mat3(1.0, 1.0, 1.0,
		0.0, -0.39465, 2.03211,
		1.13983, -0.58060, 0.0) * yuv;
	gl_FragColor = vec4(rgb, 1.0);
}
)fragmentShader";
#else
	const char* const textureStringVideo = R"fragmentShader(
varying vec2 textureOut;
uniform sampler2D tex;
void main(void) {
	gl_FragColor = texture2D(tex, textureOut);
}
)fragmentShader";

#endif
	const GLubyte* errString = gluErrorString(glGetError());

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vertexStringVideo, NULL);
	glShaderSource(fShader, 1, &textureStringVideo, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	videoEnv->program = glCreateProgram();
	glAttachShader(videoEnv->program, vShader);
	glAttachShader(videoEnv->program, fShader);

	glLinkProgram(videoEnv->program);
#if USE_YUV
	videoEnv->textureUniformY = glGetUniformLocation(videoEnv->program, "tex_y");
	videoEnv->textureUniformU = glGetUniformLocation(videoEnv->program, "tex_u");
	videoEnv->textureUniformV = glGetUniformLocation(videoEnv->program, "tex_v");
#else
	videoEnv->uniformTexture = glGetUniformLocation(videoEnv->program, "tex");
#endif
	videoEnv->vertexIn = glGetAttribLocation(videoEnv->program, "vertexIn");
	videoEnv->textureIn = glGetAttribLocation(videoEnv->program, "textureIn");

#if USE_YUV
	glGenTextures(1, &videoEnv->textureY);
	glBindTexture(GL_TEXTURE_2D, videoEnv->textureY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &videoEnv->textureU);
	glBindTexture(GL_TEXTURE_2D, videoEnv->textureU);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &videoEnv->textureV);
	glBindTexture(GL_TEXTURE_2D, videoEnv->textureV);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
	glGenTextures(1, videoEnv->Texture);
	glBindTexture(GL_TEXTURE_2D, videoEnv->Texture[0]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
}
void  draw(unsigned char* data, int width, int height, Color fc) noexcept {
#if 1
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(videoEnv.program);

	if (data) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureY);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureU);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width * height);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureV);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			data + width * height + width * height / 4);
	}
	glUniform1i(videoEnv.textureUniformY, 0);
	glUniform1i(videoEnv.textureUniformU, 1);
	glUniform1i(videoEnv.textureUniformV, 2);


	glViewport(pad_[0], pad_[1], pad_[2], pad_[3]);

	glVertexAttribPointer(videoEnv.vertexIn, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)dataGL);
	glEnableVertexAttribArray(videoEnv.vertexIn);
	glVertexAttribPointer(videoEnv.textureIn, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(dataGL + 2));

	glEnableVertexAttribArray(videoEnv.textureIn);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // GL_TRIANGLE_FAN GL_TRIANGLES



	glUseProgram(0);
#endif
#if 0
#if USE_YUV
#else
	// bool writeYUV = false;
	// if (engine) {
	//    writeYUV = engine->getWriteYUV();
	//}
	static int maxRgbDataLen = 640 * 480 * 4;
	static unsigned char* rgbData = (unsigned char*)malloc(maxRgbDataLen);
	if (width * height * 4 > maxRgbDataLen) {
		free(rgbData);
		maxRgbDataLen = width * height * 4;
		rgbData = (unsigned char*)malloc(maxRgbDataLen);
	}

	// if (writeYUV) {
	//    std::string fileName = "d_" + std::to_string(drawData->width) + "_" + std::to_string(drawData->height) + "_" + std::to_string(i)+".yuv";
	//    util::Log::WriteDataToFile(fileName, (char*)drawData->data, drawData->width * drawData->height * 3 / 2);
	//    i++;
	//}
	// step1: convert to bmp
	int yuvRet = libyuv::I420ToARGB(data, width, data + width * height, width / 2,
		data + width * height + width * height / 4, width / 2, rgbData, width * 4, width,
		height);

	// if (writeYUV) {
	//    std::string fileName = "rgb_" + std::to_string(drawData->width) + "_" + std::to_string(drawData->height) + "_" + std::to_string(i)+".rgb";
	//    util::Log::WriteDataToFile(fileName, (char*)rgbData, drawData->width * drawData->height * 4);
	//    j++;
	//}
#endif
			// glClearColor(0x9E / 255.0f, 0xEA / 255.0f, 0x6A / 255.0f, 0);
			// glClearColor(1.0f, 0, 0, 0);
	glClearColor(fc.r, fc.g, fc.b, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(videoEnv.program);
#if USE_YUV
	glUniform1i(videoEnv.textureUniformY, 0);
	glUniform1i(videoEnv.textureUniformU, 1);
	glUniform1i(videoEnv.textureUniformV, 2);
	if (data) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureY);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureU);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width * height);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, videoEnv.textureV);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			data + width * height + width * height / 4);
	}
#else
	// step2: draw it
	glUniform1i(videoEnv.uniformTexture, 0);
	if (rgbData) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, videoEnv.Texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, rgbData);
	}
#endif
	//auto markManager = oMarkManager; // std::static_pointer_cast<rainbow::MarkManagerImpl>(getEngineInstance()->getConference("")->getMarkManager());

	glViewport(pad_[0], pad_[1], pad_[2], pad_[3]);

	glVertexAttribPointer(videoEnv.vertexIn, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)dataGL);
	glEnableVertexAttribArray(videoEnv.vertexIn);
	glVertexAttribPointer(videoEnv.textureIn, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(dataGL + 2));

	glEnableVertexAttribArray(videoEnv.textureIn);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // GL_TRIANGLE_FAN GL_TRIANGLES GL_TRIANGLE_STRIP

	glUseProgram(0);
#endif
}
using namespace std;
void paint(HWND handle) noexcept {
	startupWgl(handle); // Windows System setup
	initGlew(); // OpenGL init: YUV shader, free-hand shader, arrow shader, model shader
	init(&videoEnv);
	while (true) {
		draw(yuvData_, 640, 480, color_);
		glFinish();
		// glCopyTexImage2D();
		// glCopyPixels();
		SwapBuffers(wglGetCurrentDC());
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
void calcBase(int rotation, float data[16], Size viewSize, Size resolution, float ratio[2]) noexcept {
	// x, y
	data[0] = -1.0f;
	data[1] = -1.0f;
	data[4] = 1.0f;
	data[5] = -1.0f;
	data[8] = -1.0f;
	data[9] = 1.0f;
	data[12] = 1.0f;
	data[13] = 1.0f;

	ratio[0] = 1.0f;
	ratio[1] = 1.0f;
	switch (rotation) {
	case static_cast<int>(Orientation::o0):
		// s, t
		data[2] = 0.0f;
		data[3] = 1.0f;
		data[6] = 1.0f;
		data[7] = 1.0f;
		data[10] = 0.0f;
		data[11] = 0.0f;
		data[14] = 1.0f;
		data[15] = 0.0f;
		ratio[0] = (float)viewSize.width / resolution.width;
		ratio[1] = (float)viewSize.height / resolution.height;
		break;
	case static_cast<int>(Orientation::o90):
		// s, t
		data[2] = 1.0f;
		data[3] = 1.0f;
		data[6] = 1.0f;
		data[7] = 0.0f;
		data[10] = 0.0f;
		data[11] = 1.0f;
		data[14] = 0.0f;
		data[15] = 0.0f;
		ratio[0] = (float)viewSize.width / resolution.height;
		ratio[1] = (float)viewSize.height / resolution.width;
		break;
	case static_cast<int>(Orientation::o180):
		// s, t
		data[2] = 1.0f;
		data[3] = 0.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;
		data[10] = 1.0f;
		data[11] = 1.0f;
		data[14] = 0.0f;
		data[15] = 1.0f;
		ratio[0] = (float)viewSize.width / resolution.width;
		ratio[1] = (float)viewSize.height / resolution.height;
		break;
	case static_cast<int>(Orientation::o270):
		// s, t
		data[2] = 0.0f;
		data[3] = 0.0f;
		data[6] = 0.0f;
		data[7] = 1.0f;
		data[10] = 1.0f;
		data[11] = 0.0f;
		data[14] = 1.0f;
		data[15] = 1.0f;
		ratio[0] = (float)viewSize.width / resolution.height;
		ratio[1] = (float)viewSize.height / resolution.width;
		break;
	}
}
void calcViewport(Size viewSize, Size resolution, int rotation, float vertex[16], int data[4]) noexcept {
	float ratio[2] = {};
	calcBase(rotation, vertex, viewSize, resolution, ratio);

	float scale = 1.0f;
	switch (rotation) {
	case static_cast<int>(Orientation::o0):
	case static_cast<int>(Orientation::o180):
		scale = (std::min)(ratio[0], ratio[1]);
		data[2] = (int)(resolution.width * scale);
		data[3] = (int)(resolution.height * scale);
		break;
	case static_cast<int>(Orientation::o90):
	case static_cast<int>(Orientation::o270):
		scale = (std::min)(ratio[0], ratio[1]);
		data[2] = (int)(resolution.height * scale);
		data[3] = (int)(resolution.width * scale);
		break;
	}
	int widthDiff = viewSize.width - data[2];
	int heightDiff = viewSize.height - data[3];
	data[0] = widthDiff / 2;
	data[1] = heightDiff / 2;
}
void CopengltestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int width_ = 640;
	int height_ = 480;
	Size videoSize = { width_,height_ };
	yuvData_ = (unsigned char*)malloc(width_ * height_ * 3 / 2);
	FILE* pFile = fopen("test.yuv", "rb");
	fread(yuvData_, 1, width_ * height_ * 3 / 2, pFile);
	fclose(pFile);
	hwnd_ = GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd();
	RECT rect;
	(GetDlgItem(IDC_STATIC_VIDEO))->GetClientRect(&rect);
	int viewWidth = rect.right - rect.left;
	int viewHeight = rect.bottom - rect.top;
	Size viewSize = { viewWidth,viewHeight };
	int rotation = 0;
	//displayMode_ = DisplayMode::FULL;
	displayMode_ = DisplayMode::CENTER;
	if (displayMode_ == DisplayMode::FULL) {
		float ratio[2] = {};
		calcBase(rotation, dataGL, viewSize, videoSize, ratio);
		pad_[0] = 0;
		pad_[1] = 0;
		pad_[2] = viewWidth;
		pad_[3] = viewHeight;
	}
	else if (displayMode_ == DisplayMode::CENTER) {
		calcViewport(viewSize, videoSize, rotation, dataGL, pad_);
	}
#if 0
	dataGL[0] = -1.0f;
	dataGL[1] = -1.0f;
	dataGL[4] = 1.0f;
	dataGL[5] = -1.0f;
	dataGL[8] = -1.0f;
	dataGL[9] = 1.0f;
	dataGL[12] = 1.0f;
	dataGL[13] = 1.0f;
	dataGL[2] = 0.0f;
	dataGL[3] = 1.0f;
	dataGL[6] = 1.0f;
	dataGL[7] = 1.0f;
	dataGL[10] = 0.0f;
	dataGL[11] = 0.0f;
	dataGL[14] = 1.0f;
	dataGL[15] = 0.0f;

	int top = ((viewHeight - height_) + 1) / 2;
	int left = ((viewWidth - width_) + 1) / 2;
	int pad[4] = { top, left, width_, height_ };

	memcpy(pad_, pad, sizeof(int) * 4);
#endif
	thread_ = new std::thread{ paint, hwnd_ };

	thread_->detach();

	/*std::thread([this]() noexcept {
		startupWgl();
		initGlew();
		init(&videoEnv);

		while (true) {
			draw(yuvData_, 640, 480, color_);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		}).detach();*/

}
