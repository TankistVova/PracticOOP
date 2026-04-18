#pragma once

#include "PracticeController.h"
#include "PracticeDriverApp.h"
#include "PracticeIncidents.h"
#include "PracticeLogging.h"
#include "PracticeTaxi.h"
#include <msclr/marshal_cppstd.h>

namespace PracticHub
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;
    using msclr::interop::marshal_as;

    public ref class MainForm : public Form
    {
    public:
        MainForm()
        {
            database_ = new practice::PaymentDatabase();
            gateway_ = new practice::PaymentGateway(*database_);
            coordinator_ = new practice::TaxiServiceCoordinator(*gateway_);
            controller_ = new practice::Controller();
            driverApp_ = new practice::DriverApp();
            logger_ = nullptr;
            defaultsLoaded_ = false;
            mapScale_ = 1.0f;
            mapOffset_ = Point(24, 24);

            InitializeComponent();
            UpdateControllerStateView();
            UpdateDriverAppView();
            ResetTextBoxScroll();
            this->Shown += gcnew EventHandler(this, &MainForm::MainForm_Shown);
            tabControl_->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::TabChanged);
        }

    protected:
        ~MainForm()
        {
            this->!MainForm();
        }

        !MainForm()
        {
            delete coordinator_;
            coordinator_ = nullptr;

            delete gateway_;
            gateway_ = nullptr;

            delete database_;
            database_ = nullptr;

            delete controller_;
            controller_ = nullptr;

            delete driverApp_;
            driverApp_ = nullptr;

            delete logger_;
            logger_ = nullptr;
        }

    private:
        practice::PaymentDatabase* database_;
        practice::PaymentGateway* gateway_;
        practice::TaxiServiceCoordinator* coordinator_;
        practice::Controller* controller_;
        practice::DriverApp* driverApp_;
        practice::ILogger* logger_;
        bool defaultsLoaded_;
        float mapScale_;
        Point mapOffset_;

        TabControl^ tabControl_;
        TabPage^ incidentClassTab_;
        TabPage^ incidentAnalysisTab_;
        TabPage^ incidentSearchTab_;
        TabPage^ inheritanceTab_;
        TabPage^ aggregationTab_;
        TabPage^ taxiTab_;
        TabPage^ paymentTab_;
        TabPage^ driverTab_;
        TabPage^ loggingTab_;

        TextBox^ incidentIdBox_;
        TextBox^ incidentTitleBox_;
        TextBox^ incidentDescriptionBox_;
        ComboBox^ incidentDamageBox_;
        TextBox^ incidentDateTimeBox_;
        TextBox^ incidentFileNameBox_;
        Button^ incidentSaveButton_;
        Button^ incidentValidateButton_;
        Label^ incidentPractice1SummaryLabel_;
        ListBox^ incidentPractice1DetailsList_;
        Button^ incidentAnalyzeButton_;
        Label^ incidentAnalyzeSummaryLabel_;
        ListBox^ mostDangerousList_;
        ListBox^ leastDangerousList_;
        TextBox^ incidentKeywordsBox_;
        Button^ incidentSearchButton_;
        Label^ incidentSearchSummaryLabel_;
        ListBox^ incidentSearchResultsList_;

        Button^ practice4RunButton_;
        Label^ practice4SummaryLabel_;
        ListBox^ practice4OutputList_;
        Button^ practice5RunButton_;
        Label^ practice5SummaryLabel_;
        ListBox^ practice5OutputList_;

        TextBox^ clientNameBox_;
        TextBox^ clientPhoneBox_;
        TextBox^ paymentMethodBox_;
        TextBox^ orderIdBox_;
        TextBox^ fromBox_;
        TextBox^ toBox_;
        TextBox^ priceBox_;
        Label^ taxiSummaryLabel_;
        ListBox^ paymentsList_;

        MaskedTextBox^ phoneBox_;
        TextBox^ cardBox_;
        TextBox^ ownerBox_;
        TextBox^ cvvBox_;
        Label^ stateLabel_;
        ListBox^ controllerLog_;
        Button^ phoneButton_;
        Button^ cardButton_;
        Button^ ownerButton_;
        Button^ cvvButton_;
        Button^ confirmButton_;
        Button^ resetButton_;

        TextBox^ driverNameBox_;
        TextBox^ driverIdBox_;
        Button^ registerDriverButton_;
        Button^ clearDriverButton_;
        Button^ receiveOrdersButton_;
        Button^ acceptOrderButton_;
        ComboBox^ routeVariantBox_;
        Button^ applyRouteButton_;
        Button^ advanceRouteButton_;
        Button^ finishRouteButton_;
        Button^ shiftPlusHourButton_;
        Button^ zoomInButton_;
        Button^ zoomOutButton_;
        Button^ panLeftButton_;
        Button^ panRightButton_;
        Button^ panUpButton_;
        Button^ panDownButton_;
        Label^ driverStateLabel_;
        Label^ selectedOrderLabel_;
        Label^ driverShiftLabel_;
        Label^ driverRouteLabel_;
        ListBox^ driverEventsList_;
        Panel^ roadMapPanel_;
        array<Button^>^ driverOrderButtons_;

        ComboBox^ loggerTypeBox_;
        TextBox^ logMessageBox_;
        TextBox^ logFilePathBox_;
        Label^ loggerInfoLabel_;
        ListBox^ loggerHistoryList_;

        void InitializeComponent()
        {
            this->Text = L"PracticHub - Практики 1, 2, 3, 4, 5, 6, 7, 8 и 9";
            this->ClientSize = Drawing::Size(980, 700);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->BackColor = Color::FromArgb(245, 247, 250);
            this->Font = gcnew Drawing::Font(L"Segoe UI", 10.0f);

            tabControl_ = gcnew TabControl();
            tabControl_->Dock = DockStyle::Fill;
            tabControl_->Appearance = TabAppearance::Normal;

            incidentClassTab_ = gcnew TabPage(L"Практика 1 - Инцидент");
            incidentAnalysisTab_ = gcnew TabPage(L"Практика 2 - Анализатор");
            incidentSearchTab_ = gcnew TabPage(L"Практика 3 - Поиск инцидентов");
            inheritanceTab_ = gcnew TabPage(L"Практика 4 - Наследование");
            aggregationTab_ = gcnew TabPage(L"Практика 5 - Агрегирование");
            taxiTab_ = gcnew TabPage(L"Практика 6 - Такси");
            paymentTab_ = gcnew TabPage(L"Практика 7 - Контроллер");
            driverTab_ = gcnew TabPage(L"Практика 8 - Водитель такси");
            incidentClassTab_->BackColor = this->BackColor;
            incidentAnalysisTab_->BackColor = this->BackColor;
            incidentSearchTab_->BackColor = this->BackColor;
            inheritanceTab_->BackColor = this->BackColor;
            aggregationTab_->BackColor = this->BackColor;
            taxiTab_->BackColor = this->BackColor;
            paymentTab_->BackColor = this->BackColor;
            driverTab_->BackColor = this->BackColor;

            loggingTab_ = gcnew TabPage(L"Практика 9 - Логгирование");
            loggingTab_->BackColor = this->BackColor;

            BuildPractice1Tab();
            BuildPractice2Tab();
            BuildPractice3Tab();
            BuildPractice4Tab();
            BuildPractice5Tab();
            BuildTaxiTab();
            BuildPaymentTab();
            BuildDriverTab();
            BuildLoggingTab();

            tabControl_->TabPages->Add(incidentClassTab_);
            tabControl_->TabPages->Add(incidentAnalysisTab_);
            tabControl_->TabPages->Add(incidentSearchTab_);
            tabControl_->TabPages->Add(inheritanceTab_);
            tabControl_->TabPages->Add(aggregationTab_);
            tabControl_->TabPages->Add(taxiTab_);
            tabControl_->TabPages->Add(paymentTab_);
            tabControl_->TabPages->Add(driverTab_);
            tabControl_->TabPages->Add(loggingTab_);
            Controls->Add(tabControl_);
        }

        GroupBox^ CreateSection(String^ title, Point location, Drawing::Size size)
        {
            GroupBox^ box = gcnew GroupBox();
            box->Text = title;
            box->Location = location;
            box->Size = size;
            box->BackColor = Color::White;
            return box;
        }

        Label^ CreateLabel(String^ text, Point location)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Location = location;
            label->Size = Drawing::Size(180, 26);
            return label;
        }

        Label^ CreateLabel(String^ text, Point location, int width)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Location = location;
            label->Size = Drawing::Size(width, 26);
            return label;
        }

        TextBox^ CreateTextBox(Point location, int width, String^ initial)
        {
            TextBox^ box = gcnew TextBox();
            box->Location = location;
            box->Size = Drawing::Size(width, 28);
            box->RightToLeft = System::Windows::Forms::RightToLeft::No;
            box->TextAlign = HorizontalAlignment::Left;
            box->Text = initial;
            box->SelectionStart = 0;
            box->SelectionLength = 0;
            return box;
        }

        MaskedTextBox^ CreateMaskedPhoneBox(Point location, int width)
        {
            MaskedTextBox^ box = gcnew MaskedTextBox();
            box->Location = location;
            box->Size = Drawing::Size(width, 28);
            box->Mask = L"+0(000)000-00-00";
            box->TextMaskFormat = MaskFormat::IncludePromptAndLiterals;
            box->RightToLeft = System::Windows::Forms::RightToLeft::No;
            return box;
        }

        void ResetSingleTextBox(TextBox^ box)
        {
            if (box == nullptr)
            {
                return;
            }

            box->Select(0, 0);
            box->SelectionStart = 0;
            box->SelectionLength = 0;
            box->ScrollToCaret();
            box->DeselectAll();
        }

        void ResetTextBoxScroll()
        {
            ResetSingleTextBox(incidentIdBox_);
            ResetSingleTextBox(incidentTitleBox_);
            ResetSingleTextBox(incidentDescriptionBox_);
            ResetSingleTextBox(incidentDateTimeBox_);
            ResetSingleTextBox(incidentFileNameBox_);
            ResetSingleTextBox(incidentKeywordsBox_);
            ResetSingleTextBox(clientNameBox_);
            ResetSingleTextBox(clientPhoneBox_);
            ResetSingleTextBox(paymentMethodBox_);
            ResetSingleTextBox(orderIdBox_);
            ResetSingleTextBox(fromBox_);
            ResetSingleTextBox(toBox_);
            ResetSingleTextBox(priceBox_);
            ResetSingleTextBox(cardBox_);
            ResetSingleTextBox(ownerBox_);
            ResetSingleTextBox(cvvBox_);
            ResetSingleTextBox(driverNameBox_);
            ResetSingleTextBox(driverIdBox_);
            ResetSingleTextBox(logMessageBox_);
            ResetSingleTextBox(logFilePathBox_);
        }

        void MainForm_Shown(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = tabControl_;
            LoadDefaults();
            BeginInvoke(gcnew MethodInvoker(this, &MainForm::ResetTextBoxScroll));
        }

        void TabChanged(Object^ sender, EventArgs^ e)
        {
            BeginInvoke(gcnew MethodInvoker(this, &MainForm::ResetTextBoxScroll));
        }

        void SetTextAndReset(TextBox^ box, String^ value)
        {
            box->Text = value;
            ResetSingleTextBox(box);
        }

        void LoadDefaults()
        {
            if (defaultsLoaded_)
            {
                return;
            }

            SetTextAndReset(incidentIdBox_, L"ЖЗ222333");
            SetTextAndReset(incidentTitleBox_, L"Подозрительная активность");
            SetTextAndReset(incidentDescriptionBox_, L"Сотрудник повторно запускал неизвестный исполняемый файл в системе");
            incidentDamageBox_->SelectedIndex = 1;
            SetTextAndReset(incidentDateTimeBox_, L"14.03.2025 09:40");
            SetTextAndReset(incidentFileNameBox_, L"manual_incident.log");
            SetTextAndReset(incidentKeywordsBox_, L"загрузка, файл, EXE");
            SetTextAndReset(clientNameBox_, L"Сергей Смирнов");
            SetTextAndReset(clientPhoneBox_, L"+79991234567");
            SetTextAndReset(paymentMethodBox_, L"Карта");
            SetTextAndReset(orderIdBox_, L"ORDER-1001");
            SetTextAndReset(fromBox_, L"Офис");
            SetTextAndReset(toBox_, L"Аэропорт");
            SetTextAndReset(priceBox_, L"1350");
            SetTextAndReset(cardBox_, L"1111222233334444");
            SetTextAndReset(ownerBox_, L"ИВАН ИВАНОВ");
            SetTextAndReset(cvvBox_, L"123");
            phoneBox_->Text = L"79991234567";
            SetTextAndReset(driverNameBox_, L"Иван Петров");
            SetTextAndReset(driverIdBox_, L"TX-101");
            routeVariantBox_->SelectedIndex = 0;
            SetTextAndReset(logMessageBox_, L"Попытка входа в систему");
            SetTextAndReset(logFilePathBox_, gcnew String((std::filesystem::current_path() / "practice_data" / "logs" / "security.log").string().c_str()));
            loggerTypeBox_->SelectedIndex = 0;

            defaultsLoaded_ = true;
        }

        String^ FormatIncidentLine(const practice::Incident& incident)
        {
            return String::Format(
                L"[{0}] {1} | {2} | {3}",
                gcnew String(incident.GetId().c_str()),
                gcnew String(incident.GetTitle().c_str()),
                gcnew String(practice::Incident::DamageToString(incident.GetDamage()).c_str()),
                gcnew String(incident.GetDateTime().c_str()));
        }

        std::filesystem::path GetPractice1Directory()
        {
            return std::filesystem::current_path() / "practice_data" / "practice1";
        }

        std::filesystem::path ResolvePractice1FilePath()
        {
            const std::filesystem::path inputPath = marshal_as<std::wstring>(incidentFileNameBox_->Text->Trim());
            if (inputPath.is_absolute())
            {
                return inputPath;
            }

            return GetPractice1Directory() / inputPath;
        }

        void ShowPractice1Incident(const practice::Incident& incident)
        {
            incidentPractice1DetailsList_->Items->Clear();
            incidentPractice1DetailsList_->Items->Add(L"id: " + gcnew String(incident.GetId().c_str()));

            if (!incident.IsValid())
            {
                incidentPractice1DetailsList_->Items->Add(L"Файл или данные инцидента некорректны.");
                return;
            }

            incidentPractice1DetailsList_->Items->Add(L"title: " + gcnew String(incident.GetTitle().c_str()));
            incidentPractice1DetailsList_->Items->Add(L"description: " + gcnew String(incident.GetDescription().c_str()));
            incidentPractice1DetailsList_->Items->Add(L"damage: " + gcnew String(practice::Incident::DamageToString(incident.GetDamage()).c_str()));
            incidentPractice1DetailsList_->Items->Add(L"datetime: " + gcnew String(incident.GetDateTime().c_str()));
            incidentPractice1DetailsList_->Items->Add(L"location: " + gcnew String(incident.GetLocation().c_str()));
        }

        void BuildPractice1Tab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Класс инцидента безопасности";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(420, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Практика 1 создает инцидент, сохраняет его в текстовый файл и проверяет корректность информации при повторной загрузке.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(910, 34);

            GroupBox^ editorBox = CreateSection(L"Данные инцидента", Point(22, 104), Drawing::Size(914, 250));
            editorBox->Controls->Add(CreateLabel(L"Идентификатор", Point(18, 34), 140));
            incidentIdBox_ = CreateTextBox(Point(170, 30), 220, L"");
            editorBox->Controls->Add(incidentIdBox_);

            editorBox->Controls->Add(CreateLabel(L"Наименование", Point(18, 74), 140));
            incidentTitleBox_ = CreateTextBox(Point(170, 70), 350, L"");
            editorBox->Controls->Add(incidentTitleBox_);

            editorBox->Controls->Add(CreateLabel(L"Краткое описание", Point(18, 114), 140));
            incidentDescriptionBox_ = gcnew TextBox();
            incidentDescriptionBox_->Location = Point(170, 110);
            incidentDescriptionBox_->Size = Drawing::Size(500, 58);
            incidentDescriptionBox_->Multiline = true;
            incidentDescriptionBox_->ScrollBars = ScrollBars::Vertical;
            editorBox->Controls->Add(incidentDescriptionBox_);

            editorBox->Controls->Add(CreateLabel(L"Ущерб", Point(18, 184), 140));
            incidentDamageBox_ = gcnew ComboBox();
            incidentDamageBox_->Location = Point(170, 180);
            incidentDamageBox_->Size = Drawing::Size(220, 28);
            incidentDamageBox_->DropDownStyle = ComboBoxStyle::DropDownList;
            incidentDamageBox_->Items->Add(L"Отсутствует");
            incidentDamageBox_->Items->Add(L"Незначительный");
            incidentDamageBox_->Items->Add(L"Заметный");
            incidentDamageBox_->Items->Add(L"Существенный");
            editorBox->Controls->Add(incidentDamageBox_);

            editorBox->Controls->Add(CreateLabel(L"Дата и время", Point(420, 184), 120));
            incidentDateTimeBox_ = CreateTextBox(Point(540, 180), 180, L"");
            editorBox->Controls->Add(incidentDateTimeBox_);

            GroupBox^ fileBox = CreateSection(L"Работа с файлом", Point(22, 370), Drawing::Size(914, 120));
            fileBox->Controls->Add(CreateLabel(L"Имя файла", Point(18, 42), 120));
            incidentFileNameBox_ = CreateTextBox(Point(140, 38), 360, L"");
            fileBox->Controls->Add(incidentFileNameBox_);

            incidentSaveButton_ = gcnew Button();
            incidentSaveButton_->Text = L"Сохранить инцидент";
            incidentSaveButton_->Location = Point(520, 36);
            incidentSaveButton_->Size = Drawing::Size(170, 34);
            ApplyFilledButtonStyle(incidentSaveButton_, Color::FromArgb(39, 114, 79));
            incidentSaveButton_->Click += gcnew EventHandler(this, &MainForm::SaveIncident_Click);
            fileBox->Controls->Add(incidentSaveButton_);

            incidentValidateButton_ = gcnew Button();
            incidentValidateButton_->Text = L"Проверить и загрузить файл";
            incidentValidateButton_->Location = Point(704, 36);
            incidentValidateButton_->Size = Drawing::Size(190, 34);
            incidentValidateButton_->Click += gcnew EventHandler(this, &MainForm::ValidateIncident_Click);
            fileBox->Controls->Add(incidentValidateButton_);

            Label^ fileHint = gcnew Label();
            fileHint->Text = L"Относительное имя файла будет сохранено в каталог practice_data\\practice1.";
            fileHint->Location = Point(140, 76);
            fileHint->Size = Drawing::Size(560, 24);
            fileBox->Controls->Add(fileHint);

            incidentPractice1SummaryLabel_ = gcnew Label();
            incidentPractice1SummaryLabel_->Text = L"Введите реквизиты инцидента и сохраните файл либо проверьте уже существующий.";
            incidentPractice1SummaryLabel_->Location = Point(22, 510);
            incidentPractice1SummaryLabel_->Size = Drawing::Size(914, 48);
            incidentPractice1SummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            incidentPractice1SummaryLabel_->BackColor = Color::White;

            GroupBox^ previewBox = CreateSection(L"Содержимое инцидента", Point(22, 576), Drawing::Size(914, 80));
            incidentPractice1DetailsList_ = gcnew ListBox();
            incidentPractice1DetailsList_->Location = Point(18, 24);
            incidentPractice1DetailsList_->Size = Drawing::Size(878, 40);
            previewBox->Controls->Add(incidentPractice1DetailsList_);

            incidentClassTab_->Controls->Add(header);
            incidentClassTab_->Controls->Add(hint);
            incidentClassTab_->Controls->Add(editorBox);
            incidentClassTab_->Controls->Add(fileBox);
            incidentClassTab_->Controls->Add(incidentPractice1SummaryLabel_);
            incidentClassTab_->Controls->Add(previewBox);
        }

        void BuildPractice2Tab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Анализатор инцидентов безопасности";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(450, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Практика 2 загружает список инцидентов, считает число реально загруженных записей и возвращает 3 самых опасных и 3 наименее опасных прецедента.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(910, 34);

            GroupBox^ controlBox = CreateSection(L"Запуск анализатора", Point(22, 104), Drawing::Size(914, 150));

            Label^ noteLabel = gcnew Label();
            noteLabel->Text = L"Для демонстрации используются подготовленные файлы инцидентов из каталога practice_data\\incidents.";
            noteLabel->Location = Point(18, 34);
            noteLabel->Size = Drawing::Size(760, 28);
            controlBox->Controls->Add(noteLabel);

            incidentAnalyzeButton_ = gcnew Button();
            incidentAnalyzeButton_->Text = L"Загрузить и проанализировать";
            incidentAnalyzeButton_->Location = Point(18, 82);
            incidentAnalyzeButton_->Size = Drawing::Size(260, 36);
            ApplyFilledButtonStyle(incidentAnalyzeButton_, Color::FromArgb(34, 139, 230));
            incidentAnalyzeButton_->Click += gcnew EventHandler(this, &MainForm::AnalyzeIncidents_Click);
            controlBox->Controls->Add(incidentAnalyzeButton_);

            incidentAnalyzeSummaryLabel_ = gcnew Label();
            incidentAnalyzeSummaryLabel_->Text = L"Нажмите кнопку, чтобы загрузить инциденты и получить две выборки по ущербу.";
            incidentAnalyzeSummaryLabel_->Location = Point(22, 276);
            incidentAnalyzeSummaryLabel_->Size = Drawing::Size(914, 48);
            incidentAnalyzeSummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            incidentAnalyzeSummaryLabel_->BackColor = Color::White;

            GroupBox^ dangerousBox = CreateSection(L"3 наиболее опасных", Point(22, 346), Drawing::Size(448, 280));
            mostDangerousList_ = gcnew ListBox();
            mostDangerousList_->Location = Point(18, 28);
            mostDangerousList_->Size = Drawing::Size(412, 230);
            dangerousBox->Controls->Add(mostDangerousList_);

            GroupBox^ leastBox = CreateSection(L"3 наименее опасных", Point(488, 346), Drawing::Size(448, 280));
            leastDangerousList_ = gcnew ListBox();
            leastDangerousList_->Location = Point(18, 28);
            leastDangerousList_->Size = Drawing::Size(412, 230);
            leastBox->Controls->Add(leastDangerousList_);

            incidentAnalysisTab_->Controls->Add(header);
            incidentAnalysisTab_->Controls->Add(hint);
            incidentAnalysisTab_->Controls->Add(controlBox);
            incidentAnalysisTab_->Controls->Add(incidentAnalyzeSummaryLabel_);
            incidentAnalysisTab_->Controls->Add(dangerousBox);
            incidentAnalysisTab_->Controls->Add(leastBox);
        }

        void BuildPractice3Tab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Поисковик инцидентов безопасности";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(430, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Практика 3 ищет загруженные инциденты по ключевым словам в свойстве «Краткое описание» и возвращает только совпавшие записи.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(900, 34);

            GroupBox^ searchBox = CreateSection(L"Параметры поиска", Point(22, 104), Drawing::Size(914, 170));
            searchBox->Controls->Add(CreateLabel(L"Ключевые слова", Point(18, 42), 140));
            incidentKeywordsBox_ = CreateTextBox(Point(170, 38), 560, L"");
            searchBox->Controls->Add(incidentKeywordsBox_);

            Label^ exampleLabel = gcnew Label();
            exampleLabel->Text = L"Пример: загрузка, файл, EXE";
            exampleLabel->Location = Point(170, 76);
            exampleLabel->Size = Drawing::Size(260, 24);
            searchBox->Controls->Add(exampleLabel);

            incidentSearchButton_ = gcnew Button();
            incidentSearchButton_->Text = L"Выполнить поиск";
            incidentSearchButton_->Location = Point(748, 36);
            incidentSearchButton_->Size = Drawing::Size(140, 34);
            ApplyFilledButtonStyle(incidentSearchButton_, Color::FromArgb(34, 139, 230));
            incidentSearchButton_->Click += gcnew EventHandler(this, &MainForm::SearchIncidents_Click);
            searchBox->Controls->Add(incidentSearchButton_);

            Label^ noteLabel = gcnew Label();
            noteLabel->Text = L"Поиск выполняется по демо-инцидентам из предыдущих практик с учетом регистра независимо.";
            noteLabel->Location = Point(170, 108);
            noteLabel->Size = Drawing::Size(640, 28);
            searchBox->Controls->Add(noteLabel);

            incidentSearchSummaryLabel_ = gcnew Label();
            incidentSearchSummaryLabel_->Text = L"Введите список слов через запятую и запустите поиск.";
            incidentSearchSummaryLabel_->Location = Point(22, 296);
            incidentSearchSummaryLabel_->Size = Drawing::Size(914, 48);
            incidentSearchSummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            incidentSearchSummaryLabel_->BackColor = Color::White;

            GroupBox^ resultBox = CreateSection(L"Найденные инциденты", Point(22, 366), Drawing::Size(914, 260));
            incidentSearchResultsList_ = gcnew ListBox();
            incidentSearchResultsList_->Location = Point(18, 30);
            incidentSearchResultsList_->Size = Drawing::Size(878, 210);
            resultBox->Controls->Add(incidentSearchResultsList_);

            incidentSearchTab_->Controls->Add(header);
            incidentSearchTab_->Controls->Add(hint);
            incidentSearchTab_->Controls->Add(searchBox);
            incidentSearchTab_->Controls->Add(incidentSearchSummaryLabel_);
            incidentSearchTab_->Controls->Add(resultBox);
        }

        void BuildPractice4Tab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Интерфейсы наследования для такси";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(420, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Практика 4 показывает иерархии наследования людей-участников, машин такси и обслуживания заказов с полиморфным тестированием.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(900, 34);

            GroupBox^ scenarioBox = CreateSection(L"Что входит в интерфейс", Point(22, 104), Drawing::Size(914, 180));

            Label^ scenarioText = gcnew Label();
            scenarioText->Text =
                L"1. Участники: Participant -> Person -> Client / Driver.\n"
                L"2. Машины: TaxiCar -> EconomyCar / ComfortCar.\n"
                L"3. Заказы: OrderService -> StandardOrder / PremiumOrder.\n"
                L"4. Тест проверяет выбор оплаты, принятие заказа, выбор и отмену маршрута, завершение заказа и оценку качества.";
            scenarioText->Location = Point(18, 28);
            scenarioText->Size = Drawing::Size(860, 100);
            scenarioText->AutoSize = false;
            scenarioBox->Controls->Add(scenarioText);

            practice4RunButton_ = gcnew Button();
            practice4RunButton_->Text = L"Запустить тест практики 4";
            practice4RunButton_->Location = Point(18, 138);
            practice4RunButton_->Size = Drawing::Size(240, 34);
            ApplyFilledButtonStyle(practice4RunButton_, Color::FromArgb(39, 114, 79));
            practice4RunButton_->Click += gcnew EventHandler(this, &MainForm::RunPractice4_Click);
            scenarioBox->Controls->Add(practice4RunButton_);

            practice4SummaryLabel_ = gcnew Label();
            practice4SummaryLabel_->Text = L"Нажмите кнопку, чтобы выполнить тест интерфейсов наследования для практики 4.";
            practice4SummaryLabel_->Location = Point(22, 306);
            practice4SummaryLabel_->Size = Drawing::Size(914, 48);
            practice4SummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            practice4SummaryLabel_->BackColor = Color::White;

            GroupBox^ outputBox = CreateSection(L"Результат запуска", Point(22, 376), Drawing::Size(914, 250));
            practice4OutputList_ = gcnew ListBox();
            practice4OutputList_->Location = Point(18, 28);
            practice4OutputList_->Size = Drawing::Size(878, 204);
            outputBox->Controls->Add(practice4OutputList_);

            inheritanceTab_->Controls->Add(header);
            inheritanceTab_->Controls->Add(hint);
            inheritanceTab_->Controls->Add(scenarioBox);
            inheritanceTab_->Controls->Add(practice4SummaryLabel_);
            inheritanceTab_->Controls->Add(outputBox);
        }

        void BuildPractice5Tab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Система управления услугами такси";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(440, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Практика 5 демонстрирует агрегирование независимых сущностей: клиента, заказа, уровня обслуживания, маршрутов, способа оплаты и оценки качества.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(900, 34);

            GroupBox^ scenarioBox = CreateSection(L"Что проверяет тест", Point(22, 104), Drawing::Size(914, 170));

            Label^ scenarioText = gcnew Label();
            scenarioText->Text =
                L"1. Диспетчерская агрегирует клиентов, водителей, машины и заказы.\n"
                L"2. Заказ хранит несколько вариантов маршрута и позволяет выбрать один.\n"
                L"3. Выбранный маршрут можно отменить и назначить другой.\n"
                L"4. Способ оплаты и оценка качества обслуживания сохраняются в заказе.";
            scenarioText->Location = Point(18, 30);
            scenarioText->Size = Drawing::Size(860, 92);
            scenarioText->AutoSize = false;
            scenarioBox->Controls->Add(scenarioText);

            practice5RunButton_ = gcnew Button();
            practice5RunButton_->Text = L"Запустить тест практики 5";
            practice5RunButton_->Location = Point(18, 126);
            practice5RunButton_->Size = Drawing::Size(240, 34);
            ApplyFilledButtonStyle(practice5RunButton_, Color::FromArgb(34, 139, 230));
            practice5RunButton_->Click += gcnew EventHandler(this, &MainForm::RunPractice5_Click);
            scenarioBox->Controls->Add(practice5RunButton_);

            practice5SummaryLabel_ = gcnew Label();
            practice5SummaryLabel_->Text = L"Нажмите кнопку, чтобы выполнить встроенный тест интерфейса практики 5.";
            practice5SummaryLabel_->Location = Point(22, 296);
            practice5SummaryLabel_->Size = Drawing::Size(914, 48);
            practice5SummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            practice5SummaryLabel_->BackColor = Color::White;

            GroupBox^ outputBox = CreateSection(L"Результат запуска", Point(22, 366), Drawing::Size(914, 260));
            practice5OutputList_ = gcnew ListBox();
            practice5OutputList_->Location = Point(18, 30);
            practice5OutputList_->Size = Drawing::Size(878, 210);
            outputBox->Controls->Add(practice5OutputList_);

            aggregationTab_->Controls->Add(header);
            aggregationTab_->Controls->Add(hint);
            aggregationTab_->Controls->Add(scenarioBox);
            aggregationTab_->Controls->Add(practice5SummaryLabel_);
            aggregationTab_->Controls->Add(outputBox);
        }

        void BuildTaxiTab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Композиция и ассоциация в системе такси";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(500, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Оформите поездку, завершите оплату и сохраните результат во внутренней базе.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(720, 26);

            GroupBox^ clientBox = CreateSection(L"Клиент и оплата", Point(22, 96), Drawing::Size(440, 200));
            clientBox->Controls->Add(CreateLabel(L"Имя клиента", Point(18, 34)));
            clientNameBox_ = CreateTextBox(Point(210, 30), 220, L"");
            clientBox->Controls->Add(clientNameBox_);
            clientBox->Controls->Add(CreateLabel(L"Телефон", Point(18, 74)));
            clientPhoneBox_ = CreateTextBox(Point(210, 70), 220, L"");
            clientBox->Controls->Add(clientPhoneBox_);
            clientBox->Controls->Add(CreateLabel(L"Способ оплаты", Point(18, 114)));
            paymentMethodBox_ = CreateTextBox(Point(210, 110), 220, L"");
            clientBox->Controls->Add(paymentMethodBox_);

            GroupBox^ tripBox = CreateSection(L"Данные поездки", Point(486, 96), Drawing::Size(450, 200));
            tripBox->Controls->Add(CreateLabel(L"Номер заказа", Point(18, 34)));
            orderIdBox_ = CreateTextBox(Point(210, 30), 230, L"");
            tripBox->Controls->Add(orderIdBox_);
            tripBox->Controls->Add(CreateLabel(L"Откуда", Point(18, 74)));
            fromBox_ = CreateTextBox(Point(210, 70), 230, L"");
            tripBox->Controls->Add(fromBox_);
            tripBox->Controls->Add(CreateLabel(L"Куда", Point(18, 114)));
            toBox_ = CreateTextBox(Point(210, 110), 230, L"");
            tripBox->Controls->Add(toBox_);
            tripBox->Controls->Add(CreateLabel(L"Стоимость", Point(18, 154)));
            priceBox_ = CreateTextBox(Point(210, 150), 230, L"");
            tripBox->Controls->Add(priceBox_);

            Button^ completeTripButton = gcnew Button();
            completeTripButton->Text = L"Завершить поездку";
            completeTripButton->Location = Point(22, 320);
            completeTripButton->Size = Drawing::Size(170, 40);
            completeTripButton->BackColor = Color::FromArgb(34, 139, 230);
            completeTripButton->ForeColor = Color::White;
            completeTripButton->FlatStyle = FlatStyle::Flat;
            completeTripButton->Click += gcnew EventHandler(this, &MainForm::CompleteTrip_Click);

            taxiSummaryLabel_ = gcnew Label();
            taxiSummaryLabel_->Text = L"Здесь появится информация о завершенной поездке.";
            taxiSummaryLabel_->Location = Point(22, 378);
            taxiSummaryLabel_->Size = Drawing::Size(920, 54);
            taxiSummaryLabel_->BorderStyle = BorderStyle::FixedSingle;
            taxiSummaryLabel_->BackColor = Color::White;

            Label^ dbLabel = gcnew Label();
            dbLabel->Text = L"Записи в базе оплат";
            dbLabel->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 12.0f);
            dbLabel->Location = Point(22, 452);
            dbLabel->Size = Drawing::Size(260, 30);

            paymentsList_ = gcnew ListBox();
            paymentsList_->Location = Point(22, 488);
            paymentsList_->Size = Drawing::Size(914, 140);

            taxiTab_->Controls->Add(header);
            taxiTab_->Controls->Add(hint);
            taxiTab_->Controls->Add(clientBox);
            taxiTab_->Controls->Add(tripBox);
            taxiTab_->Controls->Add(completeTripButton);
            taxiTab_->Controls->Add(taxiSummaryLabel_);
            taxiTab_->Controls->Add(dbLabel);
            taxiTab_->Controls->Add(paymentsList_);
        }

        void BuildPaymentTab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Конечный автомат оплаты";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(430, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Переходы по диаграмме: S0 -> S1 -> S5 или S0 -> S2 -> S3 -> S4 -> S5.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(540, 26);

            GroupBox^ formBox = CreateSection(L"Форма оплаты", Point(22, 96), Drawing::Size(470, 330));
            formBox->Controls->Add(CreateLabel(L"Телефон", Point(18, 42), 150));
            phoneBox_ = CreateMaskedPhoneBox(Point(175, 38), 195);
            formBox->Controls->Add(phoneBox_);
            phoneButton_ = CreateActionButton(L"Ввести", Point(380, 36));
            phoneButton_->Click += gcnew EventHandler(this, &MainForm::Phone_Click);
            formBox->Controls->Add(phoneButton_);

            formBox->Controls->Add(CreateLabel(L"Номер карты", Point(18, 92), 150));
            cardBox_ = CreateTextBox(Point(175, 88), 195, L"");
            formBox->Controls->Add(cardBox_);
            cardButton_ = CreateActionButton(L"Ввести", Point(380, 86));
            cardButton_->Click += gcnew EventHandler(this, &MainForm::Card_Click);
            formBox->Controls->Add(cardButton_);

            formBox->Controls->Add(CreateLabel(L"Владелец", Point(18, 142), 150));
            ownerBox_ = CreateTextBox(Point(175, 138), 195, L"");
            formBox->Controls->Add(ownerBox_);
            ownerButton_ = CreateActionButton(L"Ввести", Point(380, 136));
            ownerButton_->Click += gcnew EventHandler(this, &MainForm::Owner_Click);
            formBox->Controls->Add(ownerButton_);

            formBox->Controls->Add(CreateLabel(L"CVV", Point(18, 192), 150));
            cvvBox_ = CreateTextBox(Point(175, 188), 195, L"");
            formBox->Controls->Add(cvvBox_);
            cvvButton_ = CreateActionButton(L"Ввести", Point(380, 186));
            cvvButton_->Click += gcnew EventHandler(this, &MainForm::CVV_Click);
            formBox->Controls->Add(cvvButton_);

            confirmButton_ = gcnew Button();
            confirmButton_->Text = L"Подтвердить оплату";
            confirmButton_->Location = Point(175, 248);
            confirmButton_->Size = Drawing::Size(165, 38);
            confirmButton_->BackColor = Color::FromArgb(47, 158, 68);
            confirmButton_->ForeColor = Color::White;
            confirmButton_->FlatStyle = FlatStyle::Flat;
            confirmButton_->Click += gcnew EventHandler(this, &MainForm::Confirm_Click);
            formBox->Controls->Add(confirmButton_);

            resetButton_ = gcnew Button();
            resetButton_->Text = L"Сброс";
            resetButton_->Location = Point(350, 248);
            resetButton_->Size = Drawing::Size(80, 38);
            resetButton_->Click += gcnew EventHandler(this, &MainForm::Reset_Click);
            formBox->Controls->Add(resetButton_);

            GroupBox^ stateBox = CreateSection(L"Текущее состояние", Point(520, 96), Drawing::Size(416, 140));
            stateLabel_ = gcnew Label();
            stateLabel_->Text = L"S0";
            stateLabel_->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 24.0f);
            stateLabel_->Location = Point(24, 48);
            stateLabel_->Size = Drawing::Size(220, 44);
            stateBox->Controls->Add(stateLabel_);

            GroupBox^ logBox = CreateSection(L"Журнал переходов", Point(520, 256), Drawing::Size(416, 372));
            controllerLog_ = gcnew ListBox();
            controllerLog_->Location = Point(18, 32);
            controllerLog_->Size = Drawing::Size(380, 320);
            logBox->Controls->Add(controllerLog_);

            paymentTab_->Controls->Add(header);
            paymentTab_->Controls->Add(hint);
            paymentTab_->Controls->Add(formBox);
            paymentTab_->Controls->Add(stateBox);
            paymentTab_->Controls->Add(logBox);
        }

        void ApplyFilledButtonStyle(Button^ button, Color backColor)
        {
            button->BackColor = backColor;
            button->ForeColor = Color::White;
            button->FlatStyle = FlatStyle::Flat;
        }

        String^ FormatDriverMinutes(int totalMinutes)
        {
            return String::Format(L"{0} ч {1} мин", totalMinutes / 60, totalMinutes % 60);
        }

        Point TransformMapPoint(int x, int y)
        {
            return Point(
                static_cast<int>(x * mapScale_) + mapOffset_.X,
                static_cast<int>(y * mapScale_) + mapOffset_.Y);
        }

        void BuildDriverTab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Приложение для водителя такси";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(420, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Сценарий практики 8: регистрация, выбор активного заказа, работа с картой и моделирование 10 минут до финиша и 14 часов смены.";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(900, 34);

            GroupBox^ controlBox = CreateSection(L"Регистрация и прием заказов", Point(22, 96), Drawing::Size(430, 200));
            controlBox->Controls->Add(CreateLabel(L"ФИО водителя", Point(18, 38), 140));
            driverNameBox_ = CreateTextBox(Point(160, 34), 250, L"");
            controlBox->Controls->Add(driverNameBox_);

            controlBox->Controls->Add(CreateLabel(L"Табельный номер", Point(18, 78), 140));
            driverIdBox_ = CreateTextBox(Point(160, 74), 250, L"");
            controlBox->Controls->Add(driverIdBox_);

            registerDriverButton_ = gcnew Button();
            registerDriverButton_->Text = L"Зарегистрироваться";
            registerDriverButton_->Location = Point(160, 116);
            registerDriverButton_->Size = Drawing::Size(160, 36);
            ApplyFilledButtonStyle(registerDriverButton_, Color::FromArgb(39, 114, 79));
            registerDriverButton_->Click += gcnew EventHandler(this, &MainForm::RegisterDriver_Click);
            controlBox->Controls->Add(registerDriverButton_);

            clearDriverButton_ = gcnew Button();
            clearDriverButton_->Text = L"Очистить";
            clearDriverButton_->Location = Point(330, 116);
            clearDriverButton_->Size = Drawing::Size(80, 36);
            clearDriverButton_->Click += gcnew EventHandler(this, &MainForm::ClearDriver_Click);
            controlBox->Controls->Add(clearDriverButton_);

            receiveOrdersButton_ = gcnew Button();
            receiveOrdersButton_->Text = L"Показать активные заказы";
            receiveOrdersButton_->Location = Point(160, 160);
            receiveOrdersButton_->Size = Drawing::Size(250, 30);
            receiveOrdersButton_->Click += gcnew EventHandler(this, &MainForm::ReceiveOrders_Click);
            controlBox->Controls->Add(receiveOrdersButton_);

            GroupBox^ statusBox = CreateSection(L"Маршрут и состояние приложения", Point(22, 308), Drawing::Size(430, 218));
            statusBox->Controls->Add(CreateLabel(L"Вариант пути", Point(18, 36), 120));

            routeVariantBox_ = gcnew ComboBox();
            routeVariantBox_->Location = Point(160, 32);
            routeVariantBox_->Size = Drawing::Size(250, 28);
            routeVariantBox_->DropDownStyle = ComboBoxStyle::DropDownList;
            routeVariantBox_->Items->Add(L"Базовый маршрут");
            routeVariantBox_->Items->Add(L"Кратчайший путь");
            routeVariantBox_->Items->Add(L"Объезд пробок");
            statusBox->Controls->Add(routeVariantBox_);

            applyRouteButton_ = gcnew Button();
            applyRouteButton_->Text = L"Выбрать путь";
            applyRouteButton_->Location = Point(260, 70);
            applyRouteButton_->Size = Drawing::Size(150, 32);
            applyRouteButton_->Click += gcnew EventHandler(this, &MainForm::ApplyRoute_Click);
            statusBox->Controls->Add(applyRouteButton_);

            acceptOrderButton_ = gcnew Button();
            acceptOrderButton_->Text = L"Согласиться на заказ";
            acceptOrderButton_->Location = Point(18, 70);
            acceptOrderButton_->Size = Drawing::Size(220, 32);
            ApplyFilledButtonStyle(acceptOrderButton_, Color::FromArgb(34, 139, 230));
            acceptOrderButton_->Click += gcnew EventHandler(this, &MainForm::AcceptOrder_Click);
            statusBox->Controls->Add(acceptOrderButton_);

            advanceRouteButton_ = gcnew Button();
            advanceRouteButton_->Text = L"-10 минут маршрута";
            advanceRouteButton_->Location = Point(18, 112);
            advanceRouteButton_->Size = Drawing::Size(180, 32);
            advanceRouteButton_->Click += gcnew EventHandler(this, &MainForm::AdvanceRoute_Click);
            statusBox->Controls->Add(advanceRouteButton_);

            finishRouteButton_ = gcnew Button();
            finishRouteButton_->Text = L"Завершить рейс";
            finishRouteButton_->Location = Point(210, 112);
            finishRouteButton_->Size = Drawing::Size(120, 32);
            finishRouteButton_->Click += gcnew EventHandler(this, &MainForm::FinishRoute_Click);
            statusBox->Controls->Add(finishRouteButton_);

            shiftPlusHourButton_ = gcnew Button();
            shiftPlusHourButton_->Text = L"+1 час смены";
            shiftPlusHourButton_->Location = Point(340, 112);
            shiftPlusHourButton_->Size = Drawing::Size(70, 32);
            shiftPlusHourButton_->Click += gcnew EventHandler(this, &MainForm::AdvanceShift_Click);
            statusBox->Controls->Add(shiftPlusHourButton_);

            driverStateLabel_ = gcnew Label();
            driverStateLabel_->Location = Point(18, 154);
            driverStateLabel_->Size = Drawing::Size(392, 28);
            statusBox->Controls->Add(driverStateLabel_);

            selectedOrderLabel_ = gcnew Label();
            selectedOrderLabel_->Location = Point(18, 178);
            selectedOrderLabel_->Size = Drawing::Size(392, 34);
            statusBox->Controls->Add(selectedOrderLabel_);

            driverShiftLabel_ = gcnew Label();
            driverShiftLabel_->Location = Point(18, 134);
            driverShiftLabel_->Size = Drawing::Size(180, 26);
            statusBox->Controls->Add(driverShiftLabel_);

            driverRouteLabel_ = gcnew Label();
            driverRouteLabel_->Location = Point(210, 134);
            driverRouteLabel_->Size = Drawing::Size(200, 26);
            statusBox->Controls->Add(driverRouteLabel_);

            GroupBox^ mapBox = CreateSection(L"Дорожная сеть", Point(470, 96), Drawing::Size(466, 430));
            roadMapPanel_ = gcnew Panel();
            roadMapPanel_->Location = Point(18, 30);
            roadMapPanel_->Size = Drawing::Size(430, 320);
            roadMapPanel_->BackColor = Color::WhiteSmoke;
            roadMapPanel_->BorderStyle = BorderStyle::FixedSingle;
            roadMapPanel_->Paint += gcnew PaintEventHandler(this, &MainForm::RoadMapPanel_Paint);
            mapBox->Controls->Add(roadMapPanel_);

            driverOrderButtons_ = gcnew array<Button^>(3);
            for (int i = 0; i < driverOrderButtons_->Length; ++i)
            {
                Button^ orderButton = gcnew Button();
                orderButton->Size = Drawing::Size(72, 36);
                orderButton->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 8.0f);
                orderButton->Tag = i;
                orderButton->Visible = false;
                orderButton->Click += gcnew EventHandler(this, &MainForm::DriverOrder_Click);
                roadMapPanel_->Controls->Add(orderButton);
                driverOrderButtons_[i] = orderButton;
            }

            Label^ mapHint = gcnew Label();
            mapHint->Text = L"До выбора заказа масштабирование и панорамирование недоступны.";
            mapHint->Location = Point(18, 358);
            mapHint->Size = Drawing::Size(360, 24);
            mapBox->Controls->Add(mapHint);

            zoomInButton_ = CreateActionButton(L"+", Point(382, 354));
            zoomInButton_->Click += gcnew EventHandler(this, &MainForm::ZoomIn_Click);
            mapBox->Controls->Add(zoomInButton_);

            zoomOutButton_ = CreateActionButton(L"-", Point(418, 354));
            zoomOutButton_->Click += gcnew EventHandler(this, &MainForm::ZoomOut_Click);
            mapBox->Controls->Add(zoomOutButton_);

            panLeftButton_ = CreateActionButton(L"<", Point(320, 390));
            panLeftButton_->Click += gcnew EventHandler(this, &MainForm::PanLeft_Click);
            mapBox->Controls->Add(panLeftButton_);

            panRightButton_ = CreateActionButton(L">", Point(400, 390));
            panRightButton_->Click += gcnew EventHandler(this, &MainForm::PanRight_Click);
            mapBox->Controls->Add(panRightButton_);

            panUpButton_ = CreateActionButton(L"^", Point(360, 360));
            panUpButton_->Click += gcnew EventHandler(this, &MainForm::PanUp_Click);
            mapBox->Controls->Add(panUpButton_);

            panDownButton_ = CreateActionButton(L"v", Point(360, 390));
            panDownButton_->Click += gcnew EventHandler(this, &MainForm::PanDown_Click);
            mapBox->Controls->Add(panDownButton_);

            GroupBox^ logBox = CreateSection(L"Журнал сценария", Point(22, 540), Drawing::Size(914, 118));
            driverEventsList_ = gcnew ListBox();
            driverEventsList_->Location = Point(18, 28);
            driverEventsList_->Size = Drawing::Size(878, 72);
            logBox->Controls->Add(driverEventsList_);

            driverTab_->Controls->Add(header);
            driverTab_->Controls->Add(hint);
            driverTab_->Controls->Add(controlBox);
            driverTab_->Controls->Add(statusBox);
            driverTab_->Controls->Add(mapBox);
            driverTab_->Controls->Add(logBox);
        }

        void RefreshDriverEvents()
        {
            driverEventsList_->Items->Clear();

            const std::vector<std::wstring>& events = driverApp_->GetEvents();
            for (const std::wstring& entry : events)
            {
                driverEventsList_->Items->Add(gcnew String(entry.c_str()));
            }

            if (driverEventsList_->Items->Count > 0)
            {
                driverEventsList_->TopIndex = driverEventsList_->Items->Count - 1;
            }
        }

        void UpdateDriverOrderButtons()
        {
            if (driverOrderButtons_ == nullptr)
            {
                return;
            }

            const std::vector<practice::DriverOrder>& orders = driverApp_->GetOrders();
            const std::vector<practice::DriverOrderStatus>& statuses = driverApp_->GetOrderStatuses();
            const int selectedIndex = driverApp_->GetSelectedOrderIndex();
            const int currentIndex = driverApp_->GetCurrentOrderIndex();
            const int reservedIndex = driverApp_->GetReservedOrderIndex();

            for (int i = 0; i < driverOrderButtons_->Length; ++i)
            {
                Button^ orderButton = driverOrderButtons_[i];
                const practice::DriverOrder& order = orders[static_cast<std::size_t>(i)];
                const practice::DriverOrderStatus status = statuses[static_cast<std::size_t>(i)];

                orderButton->Text = gcnew String(order.id.c_str());
                orderButton->Visible = driverApp_->AreOrdersVisible();

                Point mappedPoint = TransformMapPoint(order.mapX, order.mapY);
                orderButton->Location = Point(mappedPoint.X - orderButton->Width / 2, mappedPoint.Y - orderButton->Height / 2);

                bool enabled = false;
                if (driverApp_->AreOrdersVisible() && !driverApp_->IsShiftEnded())
                {
                    if (status == practice::DriverOrderStatus::Available)
                    {
                        if (driverApp_->GetState() == practice::DriverApp::State::ReadyForOrders)
                        {
                            enabled = true;
                        }
                        else if (driverApp_->CanSelectNextOrder() && reservedIndex == -1)
                        {
                            enabled = true;
                        }
                    }

                    if (driverApp_->GetState() == practice::DriverApp::State::OrderSelected && selectedIndex == i)
                    {
                        enabled = true;
                    }

                    if (driverApp_->CanSelectNextOrder() && reservedIndex == i)
                    {
                        enabled = true;
                    }
                }

                orderButton->Enabled = enabled;

                switch (status)
                {
                case practice::DriverOrderStatus::Available:
                    orderButton->BackColor = Color::FromArgb(34, 139, 230);
                    orderButton->ForeColor = Color::White;
                    break;
                case practice::DriverOrderStatus::Current:
                    orderButton->BackColor = Color::FromArgb(47, 158, 68);
                    orderButton->ForeColor = Color::White;
                    break;
                case practice::DriverOrderStatus::Reserved:
                    orderButton->BackColor = Color::FromArgb(245, 159, 0);
                    orderButton->ForeColor = Color::Black;
                    break;
                case practice::DriverOrderStatus::Completed:
                    orderButton->BackColor = Color::Silver;
                    orderButton->ForeColor = Color::DimGray;
                    break;
                }

                if (selectedIndex == i && status == practice::DriverOrderStatus::Available)
                {
                    orderButton->BackColor = Color::FromArgb(255, 146, 43);
                    orderButton->ForeColor = Color::White;
                }

                if (currentIndex == i)
                {
                    orderButton->BackColor = Color::FromArgb(47, 158, 68);
                }
            }
        }

        void UpdateDriverAppView()
        {
            if (driverApp_ == nullptr || driverStateLabel_ == nullptr)
            {
                return;
            }

            const bool registered = driverApp_->IsRegistered();
            registerDriverButton_->Visible = !registered;
            clearDriverButton_->Visible = !registered;
            driverNameBox_->ReadOnly = registered;
            driverIdBox_->ReadOnly = registered;

            driverShiftLabel_->Text = L"Смена: " + FormatDriverMinutes(driverApp_->GetShiftMinutes());
            if (driverApp_->GetCurrentOrderIndex() == -1)
            {
                driverRouteLabel_->Text = L"Маршрут: не активен";
            }
            else
            {
                String^ routeName = driverApp_->GetRouteVariant().empty()
                    ? L"не выбран"
                    : gcnew String(driverApp_->GetRouteVariant().c_str());
                driverRouteLabel_->Text = String::Format(
                    L"Маршрут: {0}, осталось {1} мин",
                    routeName,
                    driverApp_->GetRemainingRouteMinutes());
            }

            driverStateLabel_->Text = gcnew String(driverApp_->GetStateSummary().c_str());
            selectedOrderLabel_->Text = gcnew String(driverApp_->GetSelectedOrderSummary().c_str());

            receiveOrdersButton_->Enabled = driverApp_->CanReceiveOrders();
            acceptOrderButton_->Enabled = driverApp_->CanAcceptSelectedOrder();
            routeVariantBox_->Enabled = driverApp_->CanChooseRouteVariant();
            applyRouteButton_->Enabled = driverApp_->CanChooseRouteVariant();
            advanceRouteButton_->Enabled = driverApp_->GetCurrentOrderIndex() != -1 && !driverApp_->IsShiftEnded() && driverApp_->GetRemainingRouteMinutes() > 0;
            finishRouteButton_->Enabled = driverApp_->GetCurrentOrderIndex() != -1 && !driverApp_->IsShiftEnded() && driverApp_->GetRemainingRouteMinutes() == 0;
            shiftPlusHourButton_->Enabled = !driverApp_->IsShiftEnded();

            zoomInButton_->Enabled = driverApp_->CanManipulateMap();
            zoomOutButton_->Enabled = driverApp_->CanManipulateMap();
            panLeftButton_->Enabled = driverApp_->CanManipulateMap();
            panRightButton_->Enabled = driverApp_->CanManipulateMap();
            panUpButton_->Enabled = driverApp_->CanManipulateMap();
            panDownButton_->Enabled = driverApp_->CanManipulateMap();

            RefreshDriverEvents();
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void RoadMapPanel_Paint(Object^ sender, PaintEventArgs^ e)
        {
            e->Graphics->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;
            e->Graphics->Clear(Color::WhiteSmoke);

            array<Point>^ roadNodes = gcnew array<Point>
            {
                Point(30, 60),
                Point(120, 70),
                Point(200, 35),
                Point(290, 80),
                Point(390, 45),
                Point(100, 170),
                Point(220, 190),
                Point(370, 165),
                Point(300, 250),
                Point(140, 260)
            };

            Pen^ roadPen = gcnew Pen(Color::FromArgb(110, 130, 155), 4.0f);
            for (int i = 0; i < roadNodes->Length - 1; ++i)
            {
                Point from = TransformMapPoint(roadNodes[i].X, roadNodes[i].Y);
                Point to = TransformMapPoint(roadNodes[i + 1].X, roadNodes[i + 1].Y);
                e->Graphics->DrawLine(roadPen, from, to);
            }

            e->Graphics->DrawLine(roadPen,
                TransformMapPoint(120, 70),
                TransformMapPoint(100, 170));
            e->Graphics->DrawLine(roadPen,
                TransformMapPoint(200, 35),
                TransformMapPoint(220, 190));
            e->Graphics->DrawLine(roadPen,
                TransformMapPoint(290, 80),
                TransformMapPoint(300, 250));
            e->Graphics->DrawLine(roadPen,
                TransformMapPoint(100, 170),
                TransformMapPoint(300, 250));

            for each (Point node in roadNodes)
            {
                Point mapped = TransformMapPoint(node.X, node.Y);
                e->Graphics->FillEllipse(Brushes::White, mapped.X - 6, mapped.Y - 6, 12, 12);
                e->Graphics->DrawEllipse(Pens::SteelBlue, mapped.X - 6, mapped.Y - 6, 12, 12);
            }
        }

        void BuildLoggingTab()
        {
            Label^ header = gcnew Label();
            header->Text = L"Фабрика объектов логгирования";
            header->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 16.0f);
            header->Location = Point(22, 18);
            header->Size = Drawing::Size(440, 34);

            Label^ hint = gcnew Label();
            hint->Text = L"Создайте логгер через фабрику и отправьте сообщение методом SendLog(string).";
            hint->Location = Point(24, 56);
            hint->Size = Drawing::Size(760, 26);

            GroupBox^ configBox = CreateSection(L"Параметры логгирования", Point(22, 96), Drawing::Size(914, 180));
            configBox->Controls->Add(CreateLabel(L"Тип логгера", Point(18, 38), 140));

            loggerTypeBox_ = gcnew ComboBox();
            loggerTypeBox_->Location = Point(170, 34);
            loggerTypeBox_->Size = Drawing::Size(220, 28);
            loggerTypeBox_->DropDownStyle = ComboBoxStyle::DropDownList;
            loggerTypeBox_->Items->Add(L"Консоль");
            loggerTypeBox_->Items->Add(L"Текстовый файл");
            configBox->Controls->Add(loggerTypeBox_);

            configBox->Controls->Add(CreateLabel(L"Путь к файлу", Point(18, 82), 140));
            logFilePathBox_ = CreateTextBox(Point(170, 78), 520, L"");
            configBox->Controls->Add(logFilePathBox_);

            configBox->Controls->Add(CreateLabel(L"Сообщение", Point(18, 126), 140));
            logMessageBox_ = CreateTextBox(Point(170, 122), 520, L"");
            configBox->Controls->Add(logMessageBox_);

            Button^ sendLogButton = gcnew Button();
            sendLogButton->Text = L"Создать и отправить лог";
            sendLogButton->Location = Point(710, 78);
            sendLogButton->Size = Drawing::Size(180, 40);
            sendLogButton->BackColor = Color::FromArgb(34, 139, 230);
            sendLogButton->ForeColor = Color::White;
            sendLogButton->FlatStyle = FlatStyle::Flat;
            sendLogButton->Click += gcnew EventHandler(this, &MainForm::SendLog_Click);
            configBox->Controls->Add(sendLogButton);

            loggerInfoLabel_ = gcnew Label();
            loggerInfoLabel_->Text = L"Служебная информация о созданном логгере появится здесь.";
            loggerInfoLabel_->Location = Point(22, 300);
            loggerInfoLabel_->Size = Drawing::Size(914, 54);
            loggerInfoLabel_->BorderStyle = BorderStyle::FixedSingle;
            loggerInfoLabel_->BackColor = Color::White;

            Label^ historyLabel = gcnew Label();
            historyLabel->Text = L"История логгирования";
            historyLabel->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 12.0f);
            historyLabel->Location = Point(22, 376);
            historyLabel->Size = Drawing::Size(260, 30);

            loggerHistoryList_ = gcnew ListBox();
            loggerHistoryList_->Location = Point(22, 412);
            loggerHistoryList_->Size = Drawing::Size(914, 210);

            loggingTab_->Controls->Add(header);
            loggingTab_->Controls->Add(hint);
            loggingTab_->Controls->Add(configBox);
            loggingTab_->Controls->Add(loggerInfoLabel_);
            loggingTab_->Controls->Add(historyLabel);
            loggingTab_->Controls->Add(loggerHistoryList_);
        }

        Button^ CreateActionButton(String^ text, Point location)
        {
            Button^ button = gcnew Button();
            button->Text = text;
            button->Location = location;
            button->Size = Drawing::Size(90, 30);
            return button;
        }

        std::vector<std::wstring> ParseIncidentKeywords(String^ text)
        {
            std::vector<std::wstring> keywords;
            if (String::IsNullOrWhiteSpace(text))
            {
                return keywords;
            }

            array<wchar_t>^ delimiters = gcnew array<wchar_t>{ L',', L';' };
            array<String^>^ parts = text->Split(delimiters, StringSplitOptions::RemoveEmptyEntries);

            for each (String^ part in parts)
            {
                String^ trimmed = part->Trim();
                if (!String::IsNullOrWhiteSpace(trimmed))
                {
                    keywords.push_back(marshal_as<std::wstring>(trimmed));
                }
            }

            return keywords;
        }

        void SaveIncident_Click(Object^ sender, EventArgs^ e)
        {
            if (incidentDamageBox_->SelectedItem == nullptr)
            {
                MessageBox::Show(L"Выберите значение ущерба.", L"Практика 1", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            const std::filesystem::path filePath = ResolvePractice1FilePath();
            std::filesystem::create_directories(filePath.parent_path());

            const std::wstring damageText = marshal_as<std::wstring>(safe_cast<String^>(incidentDamageBox_->SelectedItem));
            const std::optional<practice::DamageLevel> damage = practice::Incident::ParseDamage(damageText);

            if (!damage.has_value())
            {
                MessageBox::Show(L"Не удалось распознать значение ущерба.", L"Практика 1", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            practice::Incident incident(
                marshal_as<std::wstring>(incidentIdBox_->Text->Trim()),
                marshal_as<std::wstring>(incidentTitleBox_->Text->Trim()),
                marshal_as<std::wstring>(incidentDescriptionBox_->Text->Trim()),
                *damage,
                marshal_as<std::wstring>(incidentDateTimeBox_->Text->Trim()),
                filePath.filename().wstring());

            if (!incident.IsValid())
            {
                incidentPractice1SummaryLabel_->Text = L"Инцидент не сохранен: проверьте идентификатор, название, описание, ущерб, дату и имя файла.";
                ShowPractice1Incident(incident);
                return;
            }

            if (!incident.SaveToFile(filePath))
            {
                incidentPractice1SummaryLabel_->Text = L"Не удалось сохранить файл инцидента.";
                ShowPractice1Incident(practice::Incident::Invalid(filePath.filename().wstring()));
                return;
            }

            incidentPractice1SummaryLabel_->Text = L"Инцидент сохранен в файл: " + gcnew String(filePath.wstring().c_str());
            ShowPractice1Incident(incident);
        }

        void ValidateIncident_Click(Object^ sender, EventArgs^ e)
        {
            const std::filesystem::path filePath = ResolvePractice1FilePath();
            const bool isValid = practice::Incident::ValidateFile(filePath);
            const practice::Incident incident = practice::Incident::LoadFromFile(filePath);

            ShowPractice1Incident(incident);
            incidentPractice1SummaryLabel_->Text = isValid
                ? L"Файл успешно прошел проверку и был загружен."
                : L"Файл не прошел проверку. Идентификатор загруженного объекта установлен в -1.";
        }

        void AnalyzeIncidents_Click(Object^ sender, EventArgs^ e)
        {
            mostDangerousList_->Items->Clear();
            leastDangerousList_->Items->Clear();

            practice::IncidentAnalyzer analyzer;
            const std::size_t loaded = practice::LoadDemoIncidents(analyzer);
            const std::vector<practice::Incident> mostDangerous = analyzer.GetMostDangerous();
            const std::vector<practice::Incident> leastDangerous = analyzer.GetLeastDangerous();

            for (const practice::Incident& incident : mostDangerous)
            {
                mostDangerousList_->Items->Add(FormatIncidentLine(incident));
            }

            for (const practice::Incident& incident : leastDangerous)
            {
                leastDangerousList_->Items->Add(FormatIncidentLine(incident));
            }

            if (mostDangerousList_->Items->Count == 0)
            {
                mostDangerousList_->Items->Add(L"Нет данных для отображения.");
            }

            if (leastDangerousList_->Items->Count == 0)
            {
                leastDangerousList_->Items->Add(L"Нет данных для отображения.");
            }

            incidentAnalyzeSummaryLabel_->Text = String::Format(
                L"Загружено инцидентов: {0}. Наиболее опасных: {1}. Наименее опасных: {2}.",
                static_cast<int>(loaded),
                static_cast<int>(mostDangerous.size()),
                static_cast<int>(leastDangerous.size()));
        }

        void SearchIncidents_Click(Object^ sender, EventArgs^ e)
        {
            incidentSearchResultsList_->Items->Clear();

            const std::vector<std::wstring> keywords = ParseIncidentKeywords(incidentKeywordsBox_->Text);
            if (keywords.empty())
            {
                MessageBox::Show(L"Введите хотя бы одно ключевое слово.", L"Практика 3", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            practice::IncidentAnalyzer analyzer;
            const std::size_t loaded = practice::LoadDemoIncidents(analyzer);
            practice::IncidentSearcher searcher(analyzer);
            const std::vector<practice::Incident> found = searcher.SearchByKeywords(keywords);

            for (const practice::Incident& incident : found)
            {
                incidentSearchResultsList_->Items->Add(
                    String::Format(
                        L"{0} | {1}",
                        FormatIncidentLine(incident),
                        gcnew String(incident.GetDescription().c_str())));
            }

            if (incidentSearchResultsList_->Items->Count == 0)
            {
                incidentSearchResultsList_->Items->Add(L"Совпадений не найдено.");
            }

            incidentSearchSummaryLabel_->Text = String::Format(
                L"Загружено инцидентов: {0}. Ключевых слов: {1}. Найдено совпадений: {2}.",
                static_cast<int>(loaded),
                static_cast<int>(keywords.size()),
                static_cast<int>(found.size()));
        }

        void RunPractice4_Click(Object^ sender, EventArgs^ e)
        {
            practice4OutputList_->Items->Clear();

            std::ostringstream stream;
            std::streambuf* previousBuffer = std::cout.rdbuf(stream.rdbuf());
            practice::RunPractice4();
            std::cout.rdbuf(previousBuffer);

            std::istringstream lines(stream.str());
            std::string line;
            int lineCount = 0;

            while (std::getline(lines, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }

                practice4OutputList_->Items->Add(gcnew String(line.c_str()));
                ++lineCount;
            }

            practice4SummaryLabel_->Text = String::Format(
                L"Тест практики 4 выполнен. Выведено строк: {0}. Последний запуск: {1}",
                lineCount,
                DateTime::Now.ToString(L"HH:mm:ss"));
        }

        void RunPractice5_Click(Object^ sender, EventArgs^ e)
        {
            practice5OutputList_->Items->Clear();

            std::ostringstream stream;
            std::streambuf* previousBuffer = std::cout.rdbuf(stream.rdbuf());
            practice::RunPractice5();
            std::cout.rdbuf(previousBuffer);

            std::istringstream lines(stream.str());
            std::string line;
            int lineCount = 0;

            while (std::getline(lines, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }

                practice5OutputList_->Items->Add(gcnew String(line.c_str()));
                ++lineCount;
            }

            practice5SummaryLabel_->Text = String::Format(
                L"Тест практики 5 выполнен. Выведено строк: {0}. Последний запуск: {1}",
                lineCount,
                DateTime::Now.ToString(L"HH:mm:ss"));
        }

        void RegisterDriver_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = driverApp_->RegisterDriver(
                marshal_as<std::wstring>(driverNameBox_->Text),
                marshal_as<std::wstring>(driverIdBox_->Text));

            if (!ok)
            {
                MessageBox::Show(L"Используйте ФИО и табельный номер формата TX-101.", L"Практика 8", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            }
            else
            {
                driverApp_->ShowIncomingOrders();
            }

            UpdateDriverAppView();
        }

        void ClearDriver_Click(Object^ sender, EventArgs^ e)
        {
            SetTextAndReset(driverNameBox_, L"");
            SetTextAndReset(driverIdBox_, L"");
        }

        void ReceiveOrders_Click(Object^ sender, EventArgs^ e)
        {
            driverApp_->ShowIncomingOrders();
            UpdateDriverAppView();
        }

        void DriverOrder_Click(Object^ sender, EventArgs^ e)
        {
            Button^ clickedButton = safe_cast<Button^>(sender);
            const int orderIndex = safe_cast<Int32>(clickedButton->Tag);
            driverApp_->SelectOrder(static_cast<std::size_t>(orderIndex));
            UpdateDriverAppView();
        }

        void AcceptOrder_Click(Object^ sender, EventArgs^ e)
        {
            driverApp_->AcceptSelectedOrder();
            UpdateDriverAppView();
        }

        void ApplyRoute_Click(Object^ sender, EventArgs^ e)
        {
            if (routeVariantBox_->SelectedItem != nullptr)
            {
                driverApp_->ChooseRouteVariant(marshal_as<std::wstring>(safe_cast<String^>(routeVariantBox_->SelectedItem)));
            }

            UpdateDriverAppView();
        }

        void AdvanceRoute_Click(Object^ sender, EventArgs^ e)
        {
            driverApp_->AdvanceRoute(10);
            UpdateDriverAppView();
        }

        void FinishRoute_Click(Object^ sender, EventArgs^ e)
        {
            driverApp_->FinishRoute();
            UpdateDriverAppView();
        }

        void AdvanceShift_Click(Object^ sender, EventArgs^ e)
        {
            const bool wasActive = !driverApp_->IsShiftEnded();
            driverApp_->AdvanceShift(60);
            UpdateDriverAppView();

            if (wasActive && driverApp_->IsShiftEnded())
            {
                MessageBox::Show(L"Смена достигла 14 часов. Прием заказов заблокирован.", L"Практика 8", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
        }

        void ZoomIn_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapScale_ = mapScale_ >= 1.8f ? 1.8f : mapScale_ + 0.1f;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void ZoomOut_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapScale_ = mapScale_ <= 0.8f ? 0.8f : mapScale_ - 0.1f;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void PanLeft_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapOffset_.X -= 12;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void PanRight_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapOffset_.X += 12;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void PanUp_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapOffset_.Y -= 12;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void PanDown_Click(Object^ sender, EventArgs^ e)
        {
            if (!driverApp_->CanManipulateMap())
            {
                return;
            }

            mapOffset_.Y += 12;
            UpdateDriverOrderButtons();
            roadMapPanel_->Invalidate();
        }

        void CompleteTrip_Click(Object^ sender, EventArgs^ e)
        {
            double price = 0.0;
            if (!Double::TryParse(priceBox_->Text, price))
            {
                MessageBox::Show(L"Введите корректную стоимость поездки.", L"Практика 6", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            const std::string clientName = marshal_as<std::string>(clientNameBox_->Text);
            const std::string clientPhone = marshal_as<std::string>(clientPhoneBox_->Text);
            const std::string paymentMethod = marshal_as<std::string>(paymentMethodBox_->Text);
            const std::string orderId = marshal_as<std::string>(orderIdBox_->Text);
            const std::string from = marshal_as<std::string>(fromBox_->Text);
            const std::string to = marshal_as<std::string>(toBox_->Text);

            practice::Client client(clientName, clientPhone, 5.0, paymentMethod);
            practice::TaxiTrip trip(orderId, practice::Route(from, to), price);
            coordinator_->CompleteTrip(trip, client);

            taxiSummaryLabel_->Text =
                L"Поездка " + gcnew String(trip.GetOrderId().c_str()) +
                L" завершена. Маршрут: " + gcnew String(trip.GetRoute().Describe().c_str()) +
                L". Способ оплаты: " + gcnew String(client.GetPaymentMethod().c_str());

            RefreshPayments();
        }

        void RefreshPayments()
        {
            paymentsList_->Items->Clear();

            const std::vector<practice::PaymentRecord>& records = database_->Records();
            for (const practice::PaymentRecord& record : records)
            {
                String^ line =
                    gcnew String(record.orderId.c_str()) + L" | " +
                    record.amount.ToString("F2") + L" | " +
                    gcnew String(record.paymentMethod.c_str());

                paymentsList_->Items->Add(line);
            }
        }

        void UpdateControllerStateView()
        {
            const practice::Controller::State state = controller_->GetState();
            stateLabel_->Text = gcnew String(practice::Controller::StateToString(state).c_str());

            phoneButton_->Enabled = state == practice::Controller::State::S0;
            cardButton_->Enabled = state == practice::Controller::State::S0;
            ownerButton_->Enabled = state == practice::Controller::State::S2;
            cvvButton_->Enabled = state == practice::Controller::State::S3;
            confirmButton_->Enabled = state == practice::Controller::State::S1 || state == practice::Controller::State::S4;
        }

        void AddControllerLog(String^ message)
        {
            controllerLog_->Items->Add(DateTime::Now.ToString(L"HH:mm:ss") + L"  " + message);
        }

        std::string GetNormalizedPhone()
        {
            String^ masked = phoneBox_->Text;
            System::Text::StringBuilder^ digits = gcnew System::Text::StringBuilder();

            for each (wchar_t ch in masked)
            {
                if (Char::IsDigit(ch))
                {
                    digits->Append(ch);
                }
            }

            return marshal_as<std::string>(digits->ToString());
        }

        void Phone_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = controller_->InputPhone(GetNormalizedPhone());
            AddControllerLog(ok ? L"Телефон принят: S0 -> S1" : L"Телефон отклонен");
            UpdateControllerStateView();
        }

        void Card_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = controller_->InputCard(marshal_as<std::string>(cardBox_->Text));
            AddControllerLog(ok ? L"Карта принята: S0 -> S2" : L"Карта отклонена");
            UpdateControllerStateView();
        }

        void Owner_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = controller_->InputOwner(marshal_as<std::string>(ownerBox_->Text));
            AddControllerLog(ok ? L"Владелец принят: S2 -> S3" : L"Владелец отклонен");
            UpdateControllerStateView();
        }

        void CVV_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = controller_->InputCVV(marshal_as<std::string>(cvvBox_->Text));
            AddControllerLog(ok ? L"CVV принят: S3 -> S4" : L"CVV отклонен");
            UpdateControllerStateView();
        }

        void Confirm_Click(Object^ sender, EventArgs^ e)
        {
            const bool ok = controller_->ConfirmPayment();
            AddControllerLog(ok ? L"Оплата подтверждена: переход в S5" : L"Подтверждение оплаты отклонено");
            UpdateControllerStateView();
        }

        void Reset_Click(Object^ sender, EventArgs^ e)
        {
            controller_->Reset();
            AddControllerLog(L"Контроллер сброшен в состояние S0");
            UpdateControllerStateView();
        }

        void SendLog_Click(Object^ sender, EventArgs^ e)
        {
            delete logger_;
            logger_ = nullptr;

            const bool isConsole = loggerTypeBox_->SelectedIndex <= 0;
            const std::filesystem::path filePath = marshal_as<std::string>(logFilePathBox_->Text);

            std::unique_ptr<practice::ILogger> logger = practice::LoggerFactory::CreateLogger(
                isConsole ? practice::LoggerFactory::LoggerType::Console : practice::LoggerFactory::LoggerType::File,
                filePath);

            logger->SendLog(marshal_as<std::string>(logMessageBox_->Text));
            loggerInfoLabel_->Text = gcnew String(logger->GetInfo().c_str());
            loggerHistoryList_->Items->Add(
                DateTime::Now.ToString(L"HH:mm:ss") + L"  " +
                (isConsole ? L"Лог отправлен в консоль" : L"Лог отправлен в файл"));

            logger_ = logger.release();
        }
    };
}
