#include "GUI.h"


const std::string GUI::FONT_DIRECTORY = "../resources/fonts";
const std::string GUI::FONT_PATH = FONT_DIRECTORY + "/Roboto-Regular.ttf";
const float GUI::DEFAULT_FONT_SIZE = 16.0f;

const GUI::FSPath GUI::SAVE_DIRECTORY = "../save";
const GUI::FSPath GUI::SAVE_FILENAME = "settings";
const GUI::FSPath GUI::SAVE_PATH = GUI::SAVE_DIRECTORY / GUI::SAVE_FILENAME;


void GUI::initIcons() {
	this->icons.insert(
		{
			{ GUIIcons::POINT, Texture { R"(..\resources\icons\multimedia-icons\png\more.png)" } },
			{ GUIIcons::EDGE, Texture { R"(..\resources\icons\multimedia-icons\png\menu.png)" } },
			{ GUIIcons::FACE, Texture { R"(..\resources\icons\multimedia-icons\png\up-arrow-3.png)" } },
			{ GUIIcons::OBJECT, Texture { R"(..\resources\icons\multimedia-icons\png\wine-glass-1.png)" } },

			{ GUIIcons::MOVE, Texture { R"(..\resources\icons\multimedia-icons\png\move.png)" } },
			{ GUIIcons::ROTATE, Texture { R"(..\resources\icons\multimedia-icons\png\refresh.png)" } },
			{ GUIIcons::SCALE, Texture { R"(..\resources\icons\multimedia-icons\png\full.png)" } },

			{ GUIIcons::REMOVE, Texture { R"(..\resources\icons\multimedia-icons\png\trash.png)" } },

			{ GUIIcons::PERSP, Texture { R"(..\resources\icons\multimedia-icons\png\photo-camera.png)" } },
			{ GUIIcons::ORTHO, Texture { R"(..\resources\icons\multimedia-icons\png\photo-camera-1.png)" } },

			{ GUIIcons::CAMERA, Texture { R"(..\resources\icons\multimedia-icons\png\video-camera.png)" } },
			{ GUIIcons::FOCUS, Texture { R"(..\resources\icons\multimedia-icons\png\target.png)" } },
		}
	);
}

void GUI::initFont() {
	ImGuiIO& io = ImGui::GetIO();

	static const ImWchar ranges[] = { 0x0020, 0xFFFF, 0 };

	ImFontConfig icons_config;
	icons_config.MergeMode = false;

	io.Fonts->AddFontFromFileTTF(
		FONT_PATH.c_str(), this->fontSize, &icons_config, ranges);

	io.Fonts->AddFontDefault();
}

void GUI::updateServerSettings() {
	if (!Util::existsDirectory(GUI::SAVE_DIRECTORY)) return;
	if (!Util::existsFile(GUI::SAVE_PATH)) return;

	std::stringstream ss { Util::loadSettings(GUI::SAVE_PATH) };
	std::string res {};

	if (ss.str().empty()) return;

	for (std::size_t counter = 0; std::getline(ss, res, '\n'); counter++) {
		char* tmp = nullptr;

		if (counter == 0) {
			tmp = serverName;
		} else if (counter == 1) {
			tmp = login;
		} else if (counter == 2) {
			tmp = password;
		} else if (counter == 3) {
			tmp = dbName;
		} else {
			break;
		}

		for (std::size_t i = 0; i < res.size(); i++) {
			tmp[i] = res[i];
		}

		tmp[res.size()] = '\0';
	}
}

void GUI::updatePanelsByScreenSize() {
	ScreenResolution sr = Application::getInstancePtr()->getWindow().getScreen();
	this->updatePanelsByScreenSize(sr.getWidth(), sr.getHeight());
}

void GUI::updatePanelsByScreenSize(int width, int height) {
	this->updatePanelsByScreenSize(
		static_cast<float>(width), static_cast<float>(height));
}

void GUI::updatePanelsByScreenSize(float width, float height) {
	this->sizeToolBarPanel = ImVec2 { width, 50.0f };

	this->sizeToolsPanel =
		ImVec2 { 300.0f, height - this->sizeToolBarPanel.y - this->positionToolBarPanel.y };
	this->positionToolsPanel.y = this->sizeToolBarPanel.y + this->positionToolBarPanel.y;
	this->positionToolsPanel.x = width - this->sizeToolsPanel.x;

	this->sizeObjectsListPanel = this->sizeToolsPanel;
	this->positionObjectsListPanel.y = this->positionToolsPanel.y;
}

void GUI::updatePanelsUnderMenuBar(float yPosition) {
	if (this->positionToolBarPanel.y == yPosition) return;

	this->positionToolBarPanel.y = yPosition;
	this->updatePanelsByScreenSize();
}

void GUI::init() {
	this->updateServerSettings();
	this->initIcons();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	this->initFont();

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(
		Application::getInstancePtr()->getWindow().getWindowPtr(), true);
	ImGui_ImplOpenGL3_Init();

	ImGui::InitDock();

	this->updatePanelsByScreenSize();
}

void GUI::draw() {
	this->draw_NewFrame();
	this->draw_GUI();
	this->draw_Render();
}

void GUI::free() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::draw_NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::draw_GUI() {
	this->showMainMenuBar();
	this->showToolBar();
	this->showToolsPanel();
	this->showObjectsListPanel();
}

void GUI::draw_Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GUI::showSavePanel(bool* showSave) {
	static bool prevShowSave = false;
	static std::string saveTitle { "Сохранить" };
	static std::string savePanelColumn { "savePanelColumn" };

	static std::vector<const char *> sceneItems {};
	static int currentSceneItem = 0;

	static std::vector<const char *> categoryItems {};
	static int currentCategoryItem = 0;

	if (!*showSave) {
		if (prevShowSave) prevShowSave = false;
		return;
	}

	if (ImGui::Begin(saveTitle.c_str(), showSave,
					 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		if (!prevShowSave) {
			prevShowSave = true;

			Application* appThis = Application::getInstancePtr();
			ScreenResolution& sr = appThis->getWindow().getScreen();

			ImVec2 winSize = ImVec2 { 600.0f, 310.0f };
			ImVec2 winPos = ImVec2 {
				static_cast<float>(sr.getHalfWidth()) - (winSize.x / 2.0f),
				static_cast<float>(sr.getHalfHeight()) - (winSize.y / 2.0f)
			};

			ImGui::SetWindowSize(winSize, true);
			ImGui::SetWindowPos(winPos, true);


			sceneItems.clear();
			categoryItems.clear();
			currentSceneItem = 0;
			currentCategoryItem = 0;

			try {
				LoadSystem ls {
					std::string { this->serverName },
					std::string { this->login },
					std::string { this->password },
					std::string { this->dbName }
				};

				sceneItems = ls.getColumnContent("scenes", "name");
				categoryItems = ls.getColumnContent("categories", "name");
			} catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}//fi

		ImGui::Separator();

		ImGui::Columns(2, savePanelColumn.c_str(), true);
		ImGui::SetColumnWidth(0, 350.0f);
		ImGui::SetColumnWidth(1, 250.0f);

		ImGui::ListBox("Сохраненные\nсцены", &currentSceneItem,
					   &(sceneItems)[0],
					   static_cast<int>(sceneItems.size()), 10);

		ImGui::NextColumn();


		ImGui::ListBox("Категория", &currentCategoryItem,
					   &(categoryItems)[0],
					   static_cast<int>(categoryItems.size()), 8);

		ImGui::NewLine();

		static char sceneName[255] = "Scene";

		ImGui::InputText("Имя", sceneName, sizeof(sceneName),
						 ImGuiInputTextFlags_CharsNoBlank);

		ImGui::NewLine();

		if (ImGui::Button("Сохранить")) {
			try {
				SaveSystem {
					std::string { this->serverName },
					std::string { this->login },
					std::string { this->password },
					std::string { this->dbName }
				}.save(sceneName, categoryItems[currentCategoryItem]);

				prevShowSave = false;
			} catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}//fi Button
	}//fi Begin
	ImGui::End();
}

void GUI::showLoadPanel(bool* showLoad) {
	static bool prevShowLoad = false;
	static std::string loadTitle { "Открыть" };
	static std::string loadPanelColumn { "loadPanelColumn" };

	static std::vector<const char *> sceneItems {};
	static int currentSceneItem = 0;

	static std::vector<const char *> categoryItems {};
	static int currentCategoryItem = 0;

	if (!*showLoad) {
		if (prevShowLoad) prevShowLoad = false;
		return;
	}

	if (ImGui::Begin(loadTitle.c_str(), showLoad,
					 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		if (!prevShowLoad) {
			prevShowLoad = true;

			Application* appThis = Application::getInstancePtr();
			ScreenResolution& sr = appThis->getWindow().getScreen();

			ImVec2 winSize = ImVec2 { 600.0f, 310.0f };
			ImVec2 winPos = ImVec2 {
				static_cast<float>(sr.getHalfWidth()) - (winSize.x / 2.0f),
				static_cast<float>(sr.getHalfHeight()) - (winSize.y / 2.0f)
			};

			ImGui::SetWindowSize(winSize, true);
			ImGui::SetWindowPos(winPos, true);

			sceneItems.clear();
			categoryItems.clear();
			currentSceneItem = 0;

			try {
				LoadSystem ls {
					std::string { this->serverName },
					std::string { this->login },
					std::string { this->password },
					std::string { this->dbName }
				};

				categoryItems.push_back("-- all --");
				std::vector<const char*> tmpVec = ls.getColumnContent("categories", "name");
				categoryItems.insert(categoryItems.end(), tmpVec.begin(), tmpVec.end());

				if (currentCategoryItem == 0) {
					sceneItems = ls.getColumnContent("scenes", "name");
				} else {
					sceneItems = ls.getColumnContentByCategory("scenes", "name", categoryItems[currentCategoryItem]);
				}//fi
			} catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}//fi

		ImGui::Separator();

		ImGui::Columns(2, loadPanelColumn.c_str(), true);
		ImGui::SetColumnWidth(0, 350.0f);
		ImGui::SetColumnWidth(1, 250.0f);

		ImGui::ListBox("Сохраненные\nсцены", &currentSceneItem,
					   &(sceneItems)[0],
					   static_cast<int>(sceneItems.size()), 10);

		ImGui::NextColumn();

		if (ImGui::ListBox("Категория", &currentCategoryItem,
						   &(categoryItems)[0],
						   static_cast<int>(categoryItems.size()), 8)) {

			prevShowLoad = false;
		}//fi ListBox

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::Button("Загрузить")) {
			try {
				LoadSystem {
					std::string { this->serverName },
					std::string { this->login },
					std::string { this->password },
					std::string { this->dbName }
				}.load(sceneItems[currentSceneItem]);

				prevShowLoad = false;
			} catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}//fi Button

		ImGui::NewLine();
	}//fi Begin
	ImGui::End();
}

void GUI::showSettingsPanel(bool* showSettings) {
	static bool prevShowSettingsPanel = false;
	static std::string settingsTitle { "Настройки" };

	if (!*showSettings) {
		if (prevShowSettingsPanel) prevShowSettingsPanel = false;
		return;
	}

	if (ImGui::Begin(settingsTitle.c_str(), showSettings,
					 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		if (!prevShowSettingsPanel) {
			prevShowSettingsPanel = true;

			this->updateServerSettings();

			Application* appThis = Application::getInstancePtr();
			ScreenResolution& sr = appThis->getWindow().getScreen();

			ImVec2 winSize = ImVec2 { 400.0f, 215.0f };
			ImVec2 winPos = ImVec2 {
				static_cast<float>(sr.getHalfWidth()) - (winSize.x / 2.0f),
				static_cast<float>(sr.getHalfHeight()) - (winSize.y / 2.0f)
			};

			ImGui::SetWindowSize(winSize, true);
			ImGui::SetWindowPos(winPos, true);
		}//fi

		ImGui::Separator();
		ImGui::TextColored(ImVec4 { 0.5f, 0.7f, 0.9f, 1.0f }, "Сервер:");

		ImGui::InputText("сервер", serverName, sizeof(serverName),
						 ImGuiInputTextFlags_CharsNoBlank);
		ImGui::InputText("логин", login, sizeof(login),
						 ImGuiInputTextFlags_CharsNoBlank);
		ImGui::InputText("пароль", password, sizeof(password),
						 ImGuiInputTextFlags_Password |
						 ImGuiInputTextFlags_CharsNoBlank);
		ImGui::InputText("база", dbName, sizeof(dbName),
						 ImGuiInputTextFlags_CharsNoBlank);

		ImGui::NewLine();

		if (ImGui::Button("Сохранить")) {
			if (!Util::existsDirectory(GUI::SAVE_DIRECTORY)) {
				Util::createDirectories(GUI::SAVE_DIRECTORY);
			}//fi

			std::stringstream ss {};
			ss << serverName << std::endl <<
				login << std::endl <<
				password << std::endl <<
				dbName;

			Util::saveSettings(GUI::SAVE_PATH, ss.str());
		}//fi

		ImGui::Separator();
	}
	ImGui::End();
}

void GUI::showHotKeysPanel(bool* showHotKeys) {
	if (!*showHotKeys) return;

	if (ImGui::Begin("Горячие клавиши", showHotKeys, ImGuiWindowFlags_NoResize)) {
		ImGui::SetWindowSize(ImVec2 { 400.0f, 400.0f }, true);

		ImGui::Text("Tab: переключить камеру"); ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Free Camera:"); ImGui::NewLine();
		ImGui::Text("W, A, S, D, Q, E: передвижение по сцене");
		ImGui::Text("Mouse: направление движения");
		ImGui::Text("Крутить СКМ: скорость"); ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Target Camera:"); ImGui::NewLine();
		ImGui::Text("Зажать СКМ: вращение вокруг целевой точки");
		ImGui::Text("Shift + СКМ: смещение камеры и целевой точки в стороны");
		ImGui::Text("Крутить СКМ: изменение расстояния от целевой точки"); ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Проекция:"); ImGui::NewLine();
		ImGui::Text("O: ортографическая");
		ImGui::Text("P: перспективная"); ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Выделение:"); ImGui::NewLine();
		ImGui::Text("ЛКМ: выделить объект");
		ImGui::Text("Shift + ЛКМ: выделить несколько объектов");
		ImGui::Text("F: центрировать камеру на выделенных объектах");
		ImGui::Text("X: удалить выделенные объекты"); ImGui::NewLine();
		ImGui::Text("Сменить режим выделения:"); ImGui::NewLine();
		ImGui::Text("1: выделение вершин");
		ImGui::Text("2: выделение ребер");
		ImGui::Text("3: выделение граней");
		ImGui::Text("4: выделение объектов"); ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Действия с выделенными объектами:"); ImGui::NewLine();
		ImGui::Text("G: перемещение");
		ImGui::Text("R: вращение");
		ImGui::Text("S: масштабирование"); ImGui::NewLine();
		ImGui::Separator();
	}//fi Begin
	ImGui::End();
}

void GUI::showAboutWindowPanel(bool* showAboutWindow) {
	if (!*showAboutWindow) return;

	Application* appThis = Application::getInstancePtr();
	ScreenResolution& sr = appThis->getWindow().getScreen();

	if (ImGui::Begin("О проекте", showAboutWindow)) {
		ImGui::Text("Дипломный проект на тему:"); ImGui::NewLine();
		ImGui::Text("\"Разработка программной платформы интерактивной 3D-визуализации\"");
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Виталий Лифанов. Группа ЗП3-2д");
	}//fi Begin
	ImGui::End();
}

void GUI::showMainMenuBar() {
	static bool showAboutWindow = false;
	static bool showHotKeys = false;
	static bool showSettings = false;
	static bool showSave = false;
	static bool showLoad = false;

	Application* appThis = Application::getInstancePtr();

	if (ImGui::BeginMainMenuBar()) {
		this->updatePanelsUnderMenuBar(ImGui::GetWindowSize().y);

		if (ImGui::BeginMenu("Файл")) {
			if (ImGui::MenuItem("Новый")) {
				Scene newScene {};
				appThis->setScene(newScene);
				ObjectShapeCube obj {};
				appThis->getScene().addObject("cube", obj);
			}
			if (ImGui::MenuItem("Открыть", "", &showLoad)) {}
			if (ImGui::MenuItem("Сохранить", "", &showSave)) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Выход", "Ctrl + Q")) {
				appThis->quit();
			}

			ImGui::EndMenu();
		}//fi BeginMenu

		if (ImGui::BeginMenu("Редактировать")) {
			ImGui::MenuItem("Настройки", "", &showSettings);
			ImGui::EndMenu();
		}//fi BeginMenu

		if (ImGui::BeginMenu("Справка")) {
			ImGui::MenuItem("Горячие клавиши", "", &showHotKeys);
			ImGui::Separator();
			ImGui::MenuItem("О проекте", "F1", &showAboutWindow);
			ImGui::EndMenu();
		}//fi BeginMenu

		ImGui::EndMainMenuBar();

		if (ImGui::IsKeyPressed(GLFW_KEY_F1, false)) {
			showAboutWindow = !showAboutWindow;
		}

		this->showSavePanel(&showSave);
		this->showLoadPanel(&showLoad);
		this->showSettingsPanel(&showSettings);
		this->showHotKeysPanel(&showHotKeys);
		this->showAboutWindowPanel(&showAboutWindow);
	}//fi BeginMainMenuBar
}

void GUI::showToolBar() {
	std::string nameToolBar { "ToolBar" };
	std::string nameToolBarColumn { "ToolBarColumn" };

	Application* appThis = Application::getInstancePtr();
	Scene& scene = appThis->getScene();
	Selection& selection = scene.getSelection();
	SelectionSwitcher& selectionSwitcher = scene.getSelectionSwitcher();

	if (ImGui::Begin(nameToolBar.c_str(), nullptr,
					 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
					 ImGuiWindowFlags_NoScrollWithMouse)) {

		ImGui::SetWindowSize(this->sizeToolBarPanel, true);
		ImGui::SetWindowPos(this->positionToolBarPanel, true);

		ImVec2 sizeMoveButton { 29.0f, 29.0f };
		float spacing_w = 16.0f;

		ImGui::Columns(4, nameToolBarColumn.c_str(), true);
		ImGui::SetColumnWidth(0, 187.0f);
		ImGui::SetColumnWidth(1, 195.0f);
		ImGui::SetColumnWidth(2, 97.0f);
		ImGui::SetColumnWidth(3, 97.0f);

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::POINT].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			selectionSwitcher.setSelectionMode(SelectionMode::POINT);
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Режим\nвыделения\nточек (1)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::EDGE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			selectionSwitcher.setSelectionMode(SelectionMode::EDGE);
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Режим\nвыделения\nребер (2)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::FACE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			selectionSwitcher.setSelectionMode(SelectionMode::FACE);
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Режим\nвыделения\nграней (3)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::OBJECT].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			selectionSwitcher.setSelectionMode(SelectionMode::OBJECT);
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Режим\nвыделения\nобъектов (4)");
		}//fi IsItemHovered Button

		ImGui::NextColumn();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::MOVE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (selection.hasSelectedObjects()) {
				selectionSwitcher.setActionMode(SelectionActionMode::MOVING);
			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Перемещение\nвыделенных\nобъектов (G)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::ROTATE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (selection.hasSelectedObjects()) {
				selectionSwitcher.setActionMode(SelectionActionMode::ROTATION);
			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Вращение\nвыделенных\nобъектов (R)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::SCALE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (selection.hasSelectedObjects()) {
				selectionSwitcher.setActionMode(SelectionActionMode::SCALING);
			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Масштабирование\nвыделенных\nобъектов (S)");
		}//fi IsItemHovered Button

		ImGui::SameLine(0.0f, spacing_w);

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::REMOVE].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (selection.hasSelectedObjects()) {
				selection.deleting();
			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Удалить\nвыделенные\nобъекты (X)");
		}//fi IsItemHovered Button

		ImGui::NextColumn();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::PERSP].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			scene.getModel().getOrthoProj().switchToPerspective();
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Перспективный\nвид (P)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::ORTHO].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (scene.getCameraSwitcher().getType() == CameraType::TARGET) {
				scene.getModel().getOrthoProj().switchToOrthographic();
			}
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Ортографический\nвид (O)");
		}//fi IsItemHovered Button

		ImGui::NextColumn();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::CAMERA].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if ((scene.getCameraSwitcher().getType() == CameraType::TARGET &&
				 scene.getSelectionSwitcher().getActionMode() == SelectionActionMode::NONE) ||
				scene.getCameraSwitcher().getType() == CameraType::FREE) {

				scene.getCameraSwitcher().switchCamera();
			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Переключить\nтип камеры (Tab)");
		}//fi IsItemHovered Button

		ImGui::SameLine();

		ImGui::ImageButton((ImTextureID)(int64_t)this->icons[GUIIcons::FOCUS].getId(), sizeMoveButton);
		if (ImGui::IsItemClicked()) {
			if (scene.getCameraSwitcher().getType() == CameraType::TARGET &&
				scene.getSelectionSwitcher().getActionMode() == SelectionActionMode::NONE) {

				if (scene.getSelection().hasSelectedObjects()) {
					std::shared_ptr<TargetCamera> tc =
						std::dynamic_pointer_cast<TargetCamera>(scene.getCameraSwitcher().getCamera());
					tc->setTargetPosition(scene.getSelection().getCentroid());
				}//fi

			}//fi
		}//fi IsItemClicked Button
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Сфокусировать\nкамеру\nна выделенном\nобъекте (F)");
		}//fi IsItemHovered Button

		ImGui::SameLine(0.0f, spacing_w);
	} else {
		ImGui::SetWindowSize(this->sizeToolBarPanel, true);
		ImGui::SetWindowPos(this->positionToolBarPanel, true);
	}//fi Begin
	ImGui::End();
}

void GUI::showToolsPanel() {
	std::string nameTools { "Панель инструментов" };

	Application* appThis = Application::getInstancePtr();
	Scene& scene = appThis->getScene();

	if (ImGui::Begin(nameTools.c_str(), nullptr,
					 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {

		ImGui::SetWindowSize(this->sizeToolsPanel, true);
		ImGui::SetWindowPos(this->positionToolsPanel, true);

		ImGui::BeginDockspace();

		ImGui::SetNextDock(nameTools.c_str(), ImGuiDockSlot_Tab);
		if (ImGui::BeginDock("Свойства")) {
			ImGui::TextColored(ImVec4 { 0.5f, 0.7f, 0.9f, 1.0f }, "Свойства:");
			ImGui::Separator();

			if (ImGui::CollapsingHeader("FPS", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("ImGui: %s", (std::to_string(ImGui::GetIO().Framerate)).c_str());
				ImGui::Text("Editor: %s", (std::to_string(1.0f / appThis->getDeltaTime())).c_str());
			}//fi CollapsingHeader FPS

			ImGui::Separator();
		}//fi BeginDock
		ImGui::EndDock();

		ImGui::SetNextDock(nameTools.c_str(), ImGuiDockSlot_Tab);
		if (ImGui::BeginDock("Добавить объект")) {
			std::vector<const char *> listboxItems {
				"Плоскость",
				"Куб",
				"Пирамида",
			};
			static int currentListboxItem = 0;

			ImGui::Separator();
			ImGui::ListBox("Список\nобъектов", &currentListboxItem, &(listboxItems)[0],
						   static_cast<int>(listboxItems.size()),
						   static_cast<int>(listboxItems.size()));

			ImGui::NewLine();

			if (ImGui::Button("Добавить")) {
				switch (currentListboxItem) {
					case 0:
					{
						ObjectShapePlane obj {};
						scene.addObject("plane", obj);
						break;
					}
					case 1:
					{
						ObjectShapeCube obj {};
						scene.addObject("cube", obj);
						break;
					}
					case 2:
					{
						ObjectShapePyramid obj {};
						scene.addObject("pyramid", obj);
						break;
					}
					default:
						break;
				}//hctiws
			}//fi Button

			ImGui::Separator();
		}//fi BeginDock
		ImGui::EndDock();

		ImGui::EndDockspace();
	} else {
		ImGui::SetWindowSize(this->sizeToolsPanel, true);
		ImGui::SetWindowPos(this->positionToolsPanel, true);
	}//fi Begin
	ImGui::End();
}

void GUI::updateSelectedObjectsTree() {
	Selection& selection = Application::getInstancePtr()->getScene().getSelection();

	for (auto&[nameObject, object] : selection.getSelectedObjects()) {
		if (ImGui::TreeNode(nameObject.c_str())) {
			for (auto&[nameMesh, mesh] : object->getMeshes()) {
				ImGui::BulletText(nameMesh.c_str());
			}//rof meshes
			ImGui::TreePop();
		}//fi TreeNodeEx
	}//rof objects
}

void GUI::updateSceneObjectsTree() {
	Scene& scene = Application::getInstancePtr()->getScene();

	for (auto&[nameObject, object] : scene.getObjects()) {
		if (ImGui::TreeNode(nameObject.c_str())) {
			for (auto&[nameMesh, mesh] : object->getMeshes()) {
				ImGui::BulletText(nameMesh.c_str());
			}//rof meshes
			ImGui::TreePop();
		}//fi TreeNodeEx
	}//rof objects
}

void GUI::showObjectsListPanel() {
	std::string nameObjectsList { "Объекты" };

	if (ImGui::Begin(nameObjectsList.c_str(), nullptr,
					 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {

		ImGui::SetWindowSize(this->sizeObjectsListPanel, true);
		ImGui::SetWindowPos(this->positionObjectsListPanel, true);

		ImGui::BeginDockspace();

		ImGui::SetNextDock(nameObjectsList.c_str(), ImGuiDockSlot_Tab);
		if (ImGui::BeginDock("Выделены")) {
			ImGui::Separator();
			this->updateSelectedObjectsTree();
			ImGui::Separator();
		}//fi BeginDock
		ImGui::EndDock();

		ImGui::SetNextDock(nameObjectsList.c_str(), ImGuiDockSlot_Tab);
		if (ImGui::BeginDock("Сцена")) {
			ImGui::Separator();
			this->updateSceneObjectsTree();
			ImGui::Separator();
		}//fi BeginDock
		ImGui::EndDock();

		ImGui::EndDockspace();
	} else {
		ImGui::SetWindowSize(this->sizeObjectsListPanel, true);
		ImGui::SetWindowPos(this->positionObjectsListPanel, true);
	}//fi Begin
	ImGui::End();
}
