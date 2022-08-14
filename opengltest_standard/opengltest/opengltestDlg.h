
// opengltestDlg.h: 头文件
//

#pragma once
#include <GL/glew.h>
#include <thread>
#define USE_YUV 1
#define USE_RGB 0
struct VideoEnv {
	GLuint program;
	GLuint textureUniformY, textureUniformU, textureUniformV;//YUV
	GLuint textureY, textureU, textureV;
	GLuint vertexIn;
	GLuint textureIn;
	GLuint uniformTexture;
	GLuint Texture[1];
};
struct Color {
	float r;
	float g;
	float b;
};
struct Size {
	int width;
	int height;
};
enum class DisplayMode { CENTER, FULL };
enum class Orientation { o0 = 0, o90 = 90, o180 = 180, o270 = 270 };
// CopenglMfcRenderDlg 对话框

// CopengltestDlg 对话框
class CopengltestDlg : public CDialogEx
{
// 构造
public:
	CopengltestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENGLTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	std::thread* thread_ = nullptr;
	DisplayMode displayMode_;
};
