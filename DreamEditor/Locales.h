#pragma once
// Since we will not actually need a GUID for interface strings, because we do not ever need to compare them
// And  Also since for peculiar reason a global string array defined here isn't just working
// We could just use Macro Definitions instead
#define Locale_Chn	// Change Locale selection here

// >>> English Locale
#if defined(Locale_Eng)
// Interface String Definitions
// Main Menu Bar
#define Interface_Filemenu L"FILE"	// Visual Studio Style
#define Interface_Windowmenu L"WINDOW"
#define Interface_Helpmenu L"HELP"
// Main Menu Popups
#define Interface_PopupMenu_Save L"Save"
#define Interface_PopupMenu_Exit L"Quit"
#define Interface_PopupMenu_Fullscreen L"Fullscreen"
#define Interface_PopupMenu_Shortcuts L"Shortcuts"
#define Interface_PopupMenu_Manual L"Manual"
#define Interface_PopupMenu_About L"About"
// Popup Screens_About Screen
#define Interface_AboutScreen_Original L"Original Author: Chaojian Zhang"
#define Interface_AboutScreen_Contact L"Contact: szinubuntu@gmail.com"
// Popup Screens_Shortcuts Screen

// Popup Menu_MeshCreationPopup
#define Interface_MeshCreationPop_AddMesh L"Add MeshNode"
#define Interface_MeshCreationPop_RotatingCube L"Rotating Cube"
#define Interface_MeshCreationPop_AnywhereDoor L"Anywhere Door"
// Other Popups
#define Interface_PopupMenu_AddMeshNode L"Add MeshNode"
// Property Panels
#define Interface_PropertyPanel_Document L"Document:"
#define Interface_PropertyPanel_ImageNode L"Image Node:"
#define Interface_PropertyPanel_MeshNode L"Mesh Node:"
#define Interface_PropertyPanel_TextNode L"Text Node:"
#define Interface_PropertyPanel_CreationTime L"Date Created:"
#define Interface_PropertyPanel_EditTime L"Date Modified:"
#define Interface_PropertyPanel_Transformation L"Transformation:"
#define Interface_PropertyPanel_TransformationX L"X:"
#define Interface_PropertyPanel_TransformationY L"Y:"
#define Interface_PropertyPanel_TransformationZ L"Z:"
#define Interface_PropertyPanel_MeshName L"Mesh Name:"
#define Interface_PropertyPanel_Comment L"Comment:"
#define Interface_PropertyPanel_Export L"Export"
// Map Editor
#define Interface_MapEditor_ExportMap L"Export Map"

// System Internal Outputs
#define TextFile_Export_CreateTime L"Created: "
#define TextFile_Export_Aux_Newline L"\n"

// Canvas Interface Informations
#define Canvas_Information_Welcome L"Welcome!"	// Appear when first open application
#define Canvas_Information_UsageTip_CreateTextNode L"Use Ctrl+LMB click on the ground to place a text node."	// Appear when user click on blank space
#define Canvas_Information_UsageTip_ShiftToAccelerate L"Use Shift to accelerate."
#define Canvas_Information_LoadingMesh_Begin L"Streaming mesh..."
#define Canvas_Information_LoadingMesh_Finish L"Mesh loaded!"
#define Canvas_Information_Save L"Document saved!"
// ///////////////////////////////////////////////////////////////////English Locale

// >>> Chinese Locale
#elif defined(Locale_Chn)
// Interface String Definitions
// Main Menu Bar
#define Interface_Filemenu L"文件"	// Visual Studio Style
#define Interface_Windowmenu L"窗口"
#define Interface_Helpmenu L"帮助"
// Main Menu Popups
#define Interface_PopupMenu_Save L"保存"
#define Interface_PopupMenu_Exit L"离开"
#define Interface_PopupMenu_Fullscreen L"全屏"
#define Interface_PopupMenu_Shortcuts L"热键"
#define Interface_PopupMenu_Manual L"手册"
#define Interface_PopupMenu_About L"关于"
// Popup Screens_About Screen
#define Interface_AboutScreen_Original L"原作者: 张朝剑"
#define Interface_AboutScreen_Contact L"电邮：szinubuntu@gmail.com"
// Popup Screens_Shortcuts Screen

// Popup Menu_MeshCreationPopup
#define Interface_MeshCreationPop_AddMesh L"添加物体节点"
#define Interface_MeshCreationPop_RotatingCube L"旋转方块"
#define Interface_MeshCreationPop_AnywhereDoor L"任意门"
// Other Popups
#define Interface_PopupMenu_AddMeshNode L"添加物体节点"
// Property Panels
#define Interface_PropertyPanel_Document L"文档:"
#define Interface_PropertyPanel_ImageNode L"图片节点:"
#define Interface_PropertyPanel_MeshNode L"物体节点:"
#define Interface_PropertyPanel_TextNode L"文字节点:"
#define Interface_PropertyPanel_CreationTime L"创建日期:"
#define Interface_PropertyPanel_EditTime L"修改日期:"
#define Interface_PropertyPanel_Transformation L"变换:"
#define Interface_PropertyPanel_TransformationX L"X:"
#define Interface_PropertyPanel_TransformationY L"Y:"
#define Interface_PropertyPanel_TransformationZ L"Z:"
#define Interface_PropertyPanel_MeshName L"物体名称:"
#define Interface_PropertyPanel_Comment L"附言:"	// L"留言:"
#define Interface_PropertyPanel_Export L"导出"
// Map Editor
#define Interface_MapEditor_ExportMap L"导出地图"

// System Internal Outputs
#define TextFile_Export_CreateTime L"创建："
#define TextFile_Export_Aux_Newline L"\n"

// Canvas Interface Informations
#define Canvas_Information_Welcome L"欢迎!"	// Appear when first open application
#define Canvas_Information_UsageTip_CreateTextNode L"使用Ctrl+LMB在平地上单击以创建一个文字节点."	// Appear when user click on blank space
#define Canvas_Information_UsageTip_ShiftToAccelerate L"使用Shift键加速移动."
#define Canvas_Information_LoadingMesh_Begin L"载入物体..."
#define Canvas_Information_LoadingMesh_Finish L"物体已载入!"
#define Canvas_Information_Save L"文档已保存!"

// ///////////////////////////////////////////////////////////////////Chinese Locale

// >>> Opps, forget to define the macros
#else
static_assert(false);
#endif