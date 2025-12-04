#pragma once

namespace OpenGL
{
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    /// <summary>
    /// Summary for ToolWindow
    /// </summary>
    public ref class ToolWindow : public Form
    {
    public:
        // Static properties for light and material settings
        static bool MoveLightEnabled = true;
        static float SpecularStrength = 4.0f;
        static float SpecularColorR = 1.0f;
        static float SpecularColorG = 1.0f;
        static float SpecularColorB = 1.0f;
        static bool TransformEnabled = false;
        static bool TranslateEnabled = false;
        static bool RotateEnabled = false;
        static bool ScaleEnabled = false;
        static float WaterScaleFrequency = 4.0f;
        static float WaterScaleAmplitude = 0.01f;
        static bool WaterSceneEnabled = false;
        static bool WireframeRenderEnabled = false;
        static bool TintBlueEnabled = false;
        static bool SpaceSceneEnabled = false;
        static bool ResetLightPositionRequested = false;
        static float DefaultLightPositionX = 0.0f;
        static float DefaultLightPositionY = 0.5f;
        static float DefaultLightPositionZ = 1.0f;

        ToolWindow(void)
        {
            InitializeComponent();
            InitializeValues();
        }

    protected:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~ToolWindow()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        // Move Light Section
        RadioButton^ radioButtonMoveLight;
        Button^ buttonResetLightPosition;

        // Specular Section
        TrackBar^ trackBarSpecularStrength;
        Label^ labelSpecularStrength;
        TrackBar^ trackBarSpecularColorR;
        TrackBar^ trackBarSpecularColorG;
        TrackBar^ trackBarSpecularColorB;
        Label^ labelSpecularColorR;
        Label^ labelSpecularColorG;
        Label^ labelSpecularColorB;

        // Transform Section
        RadioButton^ radioButtonTransform;
        CheckBox^ checkBoxTranslate;
        CheckBox^ checkBoxRotate;
        CheckBox^ checkBoxScale;
        Button^ buttonResetTransform;

        // Water Scale Section
        RadioButton^ radioButtonWaterScale;
        TrackBar^ trackBarWaterScaleFrequency;
        TrackBar^ trackBarWaterScaleAmplitude;
        Label^ labelWaterScaleFrequency;
        Label^ labelWaterScaleAmplitude;

        // Render Options Section
        RadioButton^ radioButtonSpaceScene;
        CheckBox^ checkBoxWireframeRender;
        CheckBox^ checkBoxTintBlue;


        /// <summary>
        /// Required designer variable.
        /// </summary>
        System::ComponentModel::Container^ components;

        void InitializeValues(void)
        {
            MoveLightEnabled = radioButtonMoveLight->Checked;
            SpecularStrength = static_cast<float>(trackBarSpecularStrength->Value) / 10.0f;
            SpecularColorR = static_cast<float>(trackBarSpecularColorR->Value) / 100.0f;
            SpecularColorG = static_cast<float>(trackBarSpecularColorG->Value) / 100.0f;
            SpecularColorB = static_cast<float>(trackBarSpecularColorB->Value) / 100.0f;
            TranslateEnabled = checkBoxTranslate->Checked;
            RotateEnabled = checkBoxRotate->Checked;
            ScaleEnabled = checkBoxScale->Checked;
            WaterScaleFrequency = static_cast<float>(trackBarWaterScaleFrequency->Value) / 10.0f;
            WaterScaleAmplitude = static_cast<float>(trackBarWaterScaleAmplitude->Value) / 100.0f;
            WireframeRenderEnabled = checkBoxWireframeRender->Checked;
            SpaceSceneEnabled = radioButtonSpaceScene->Checked;

            // Update label text with initial values
            labelSpecularStrength->Text = String::Format(
                "Specular Strength                                    {0:0.00}", SpecularStrength);
            labelSpecularColorR->Text = String::Format("Specular Color R                                    {0:0.00}",
                                                       SpecularColorR);
            labelSpecularColorG->Text = String::Format("Specular Color G                                    {0:0.00}",
                                                       SpecularColorG);
            labelSpecularColorB->Text = String::Format("Specular Color B                                    {0:0.00}",
                                                       SpecularColorB);
            labelWaterScaleFrequency->Text = String::Format(
                "Frequency                                              {0:0.00}", WaterScaleFrequency);
            labelWaterScaleAmplitude->Text = String::Format(
                "Amplitude                                              {0:0.00}", WaterScaleAmplitude);
            TintBlueEnabled = checkBoxTintBlue->Checked;
        }

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            // Move Light Section
            this->radioButtonMoveLight = (gcnew RadioButton());
            this->buttonResetLightPosition = (gcnew Button());

            // Specular Section
            this->trackBarSpecularStrength = (gcnew TrackBar());
            this->labelSpecularStrength = (gcnew Label());
            this->trackBarSpecularColorR = (gcnew TrackBar());
            this->trackBarSpecularColorG = (gcnew TrackBar());
            this->trackBarSpecularColorB = (gcnew TrackBar());
            this->labelSpecularColorR = (gcnew Label());
            this->labelSpecularColorG = (gcnew Label());
            this->labelSpecularColorB = (gcnew Label());

            // Transform Section
            this->radioButtonTransform = (gcnew RadioButton());
            this->checkBoxTranslate = (gcnew CheckBox());
            this->checkBoxRotate = (gcnew CheckBox());
            this->checkBoxScale = (gcnew CheckBox());
            this->buttonResetTransform = (gcnew Button());

            // Water Scale Section
            this->radioButtonWaterScale = (gcnew RadioButton());
            this->trackBarWaterScaleFrequency = (gcnew TrackBar());
            this->trackBarWaterScaleAmplitude = (gcnew TrackBar());
            this->labelWaterScaleFrequency = (gcnew Label());
            this->labelWaterScaleAmplitude = (gcnew Label());

            // Space Scene Section
            this->radioButtonSpaceScene = (gcnew RadioButton());
            this->checkBoxWireframeRender = (gcnew CheckBox());
            this->checkBoxTintBlue = (gcnew CheckBox());

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularStrength))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorR))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorG))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorB))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarWaterScaleFrequency))->
                BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarWaterScaleAmplitude))->
                BeginInit();
            this->SuspendLayout();

            int yPos = 10;

            // Move Light Section
            this->radioButtonMoveLight->AutoSize = true;
            this->radioButtonMoveLight->Location = Point(10, yPos);
            this->radioButtonMoveLight->Text = L"Move Light";
            this->radioButtonMoveLight->Checked = true;
            this->radioButtonMoveLight->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::radioButtonMoveLight_CheckedChanged);
            this->Controls->Add(this->radioButtonMoveLight);
            yPos += 25;

            this->buttonResetLightPosition->Location = Point(30, yPos);
            this->buttonResetLightPosition->Text = L"Reset Light Position";
            this->buttonResetLightPosition->Size = Drawing::Size(220, 25);
            this->buttonResetLightPosition->Click += gcnew EventHandler(
                this, &ToolWindow::buttonResetLightPosition_Click);
            this->Controls->Add(this->buttonResetLightPosition);
            yPos += 35;

            // Specular Strength
            this->labelSpecularStrength->Location = Point(10, yPos);
            this->labelSpecularStrength->Text = L"Specular Strength                                    1";
            this->labelSpecularStrength->Size = Drawing::Size(360, 20);
            this->Controls->Add(this->labelSpecularStrength);
            yPos += 25;

            this->trackBarSpecularStrength->Location = Point(10, yPos);
            this->trackBarSpecularStrength->Size = Drawing::Size(350, 45);
            this->trackBarSpecularStrength->Minimum = 0;
            this->trackBarSpecularStrength->Maximum = 100;
            this->trackBarSpecularStrength->Value = 40;
            this->trackBarSpecularStrength->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarSpecularStrength_ValueChanged);
            this->Controls->Add(this->trackBarSpecularStrength);
            yPos += 60;

            // Specular Color R
            this->labelSpecularColorR->Location = Point(10, yPos);
            this->labelSpecularColorR->Text = L"Specular Color R                                    3.00";
            this->labelSpecularColorR->Size = Drawing::Size(360, 20);
            this->Controls->Add(this->labelSpecularColorR);
            yPos += 25;

            this->trackBarSpecularColorR->Location = Point(10, yPos);
            this->trackBarSpecularColorR->Size = Drawing::Size(350, 45);
            this->trackBarSpecularColorR->Minimum = 0;
            this->trackBarSpecularColorR->Maximum = 300;
            this->trackBarSpecularColorR->Value = 100;
            this->trackBarSpecularColorR->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarSpecularColorR_ValueChanged);
            this->Controls->Add(this->trackBarSpecularColorR);
            yPos += 60;

            // Specular Color G
            this->labelSpecularColorG->Location = Point(10, yPos);
            this->labelSpecularColorG->Text = L"G                                                  1.68";
            this->labelSpecularColorG->Size = Drawing::Size(360, 20);
            this->Controls->Add(this->labelSpecularColorG);
            yPos += 25;

            this->trackBarSpecularColorG->Location = Point(10, yPos);
            this->trackBarSpecularColorG->Size = Drawing::Size(350, 45);
            this->trackBarSpecularColorG->Minimum = 0;
            this->trackBarSpecularColorG->Maximum = 300;
            this->trackBarSpecularColorG->Value = 100;
            this->trackBarSpecularColorG->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarSpecularColorG_ValueChanged);
            this->Controls->Add(this->trackBarSpecularColorG);
            yPos += 60;

            // Specular Color B
            this->labelSpecularColorB->Location = Point(10, yPos);
            this->labelSpecularColorB->Text = L"B                                                  1.00";
            this->labelSpecularColorB->Size = Drawing::Size(360, 20);
            this->Controls->Add(this->labelSpecularColorB);
            yPos += 25;

            this->trackBarSpecularColorB->Location = Point(10, yPos);
            this->trackBarSpecularColorB->Size = Drawing::Size(350, 45);
            this->trackBarSpecularColorB->Minimum = 0;
            this->trackBarSpecularColorB->Maximum = 300;
            this->trackBarSpecularColorB->Value = 100;
            this->trackBarSpecularColorB->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarSpecularColorB_ValueChanged);
            this->Controls->Add(this->trackBarSpecularColorB);
            yPos += 60;

            // Transform Section
            this->radioButtonTransform->AutoSize = true;
            this->radioButtonTransform->Location = Point(10, yPos);
            this->radioButtonTransform->Text = L"Transform";
            this->radioButtonTransform->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::radioButtonTransform_CheckedChanged);
            this->Controls->Add(this->radioButtonTransform);
            yPos += 25;

            this->checkBoxTranslate->AutoSize = true;
            this->checkBoxTranslate->Location = Point(30, yPos);
            this->checkBoxTranslate->Text = L"Translate";
            this->checkBoxTranslate->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::checkBoxTranslate_CheckedChanged);
            this->Controls->Add(this->checkBoxTranslate);
            yPos += 25;

            this->checkBoxRotate->AutoSize = true;
            this->checkBoxRotate->Location = Point(30, yPos);
            this->checkBoxRotate->Text = L"Rotate";
            this->checkBoxRotate->CheckedChanged += gcnew
                EventHandler(this, &ToolWindow::checkBoxRotate_CheckedChanged);
            this->Controls->Add(this->checkBoxRotate);
            yPos += 25;

            this->checkBoxScale->AutoSize = true;
            this->checkBoxScale->Location = Point(30, yPos);
            this->checkBoxScale->Text = L"Scale";
            this->checkBoxScale->CheckedChanged += gcnew EventHandler(this, &ToolWindow::checkBoxScale_CheckedChanged);
            this->Controls->Add(this->checkBoxScale);
            yPos += 25;

            this->buttonResetTransform->Location = Point(30, yPos);
            this->buttonResetTransform->Text = L"Reset Transform";
            this->buttonResetTransform->Size = Drawing::Size(220, 25);
            this->buttonResetTransform->Click += gcnew EventHandler(this, &ToolWindow::buttonResetTransform_Click);
            this->Controls->Add(this->buttonResetTransform);
            yPos += 40;

            // Water Scale Section
            this->radioButtonWaterScale->AutoSize = true;
            this->radioButtonWaterScale->Location = Point(10, yPos);
            this->radioButtonWaterScale->Text = L"Water Scene";
            this->radioButtonWaterScale->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::radioButtonWaterScale_CheckedChanged);
            this->Controls->Add(this->radioButtonWaterScale);
            yPos += 25;

            this->labelWaterScaleFrequency->Location = Point(30, yPos);
            this->labelWaterScaleFrequency->Text = L"Frequency                                              4.00";
            this->labelWaterScaleFrequency->Size = Drawing::Size(310, 20);
            this->Controls->Add(this->labelWaterScaleFrequency);
            yPos += 25;

            this->trackBarWaterScaleFrequency->Location = Point(30, yPos);
            this->trackBarWaterScaleFrequency->Size = Drawing::Size(320, 45);
            this->trackBarWaterScaleFrequency->Minimum = 0;
            this->trackBarWaterScaleFrequency->Maximum = 40;
            this->trackBarWaterScaleFrequency->Value = 40;
            this->trackBarWaterScaleFrequency->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarWaterScaleFrequency_ValueChanged);
            this->Controls->Add(this->trackBarWaterScaleFrequency);
            yPos += 60;

            this->labelWaterScaleAmplitude->Location = Point(30, yPos);
            this->labelWaterScaleAmplitude->Text = L"Amplitude                                              0.01";
            this->labelWaterScaleAmplitude->Size = Drawing::Size(310, 20);
            this->Controls->Add(this->labelWaterScaleAmplitude);
            yPos += 25;

            this->trackBarWaterScaleAmplitude->Location = Point(30, yPos);
            this->trackBarWaterScaleAmplitude->Size = Drawing::Size(320, 45);
            this->trackBarWaterScaleAmplitude->Minimum = 0;
            this->trackBarWaterScaleAmplitude->Maximum = 100;
            this->trackBarWaterScaleAmplitude->Value = 1;
            this->trackBarWaterScaleAmplitude->ValueChanged += gcnew EventHandler(
                this, &ToolWindow::trackBarWaterScaleAmplitude_ValueChanged);
            this->Controls->Add(this->trackBarWaterScaleAmplitude);
            yPos += 60;

            this->checkBoxWireframeRender->AutoSize = true;
            this->checkBoxWireframeRender->Location = Point(30, yPos);
            this->checkBoxWireframeRender->Text = L"Wireframe Render";
            this->checkBoxWireframeRender->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::checkBoxWireframeRender_CheckedChanged);
            this->Controls->Add(this->checkBoxWireframeRender);
            yPos += 25;

            this->checkBoxTintBlue->AutoSize = true;
            this->checkBoxTintBlue->Location = Point(30, yPos);
            this->checkBoxTintBlue->Text = L"Tint Blue";
            this->checkBoxTintBlue->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::checkBoxTintBlue_CheckedChanged);
            this->Controls->Add(this->checkBoxTintBlue);
            yPos += 25;

            // Space Scene Section
            this->radioButtonSpaceScene->AutoSize = true;
            this->radioButtonSpaceScene->Location = Point(10, yPos);
            this->radioButtonSpaceScene->Text = L"Space Scene";
            this->radioButtonSpaceScene->CheckedChanged += gcnew EventHandler(
                this, &ToolWindow::radioButtonSpaceScene_CheckedChanged);
            this->Controls->Add(this->radioButtonSpaceScene);
            yPos += 25;

            // Form properties
            this->AutoScaleDimensions = SizeF(8, 16);
            this->AutoScaleMode = Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = Drawing::Size(380, yPos + 20);
            this->FormBorderStyle = Windows::Forms::FormBorderStyle::FixedToolWindow;
            this->Name = L"ToolWindow";
            this->Text = L"ToolWindow";
            this->TopMost = true;
            this->AutoScroll = true;

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularStrength))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorR))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorG))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarSpecularColorB))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarWaterScaleFrequency))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarWaterScaleAmplitude))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion

        void radioButtonMoveLight_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            MoveLightEnabled = radioButtonMoveLight->Checked;
        }

        void radioButtonTransform_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            TransformEnabled = radioButtonTransform->Checked;
        }

        void radioButtonWaterScale_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            WaterSceneEnabled = radioButtonWaterScale->Checked;
        }

        void radioButtonSpaceScene_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            SpaceSceneEnabled = radioButtonSpaceScene->Checked;
        }

        void buttonResetLightPosition_Click(Object^ sender, EventArgs^ e)
        {
            // Signal that light position should be reset
            ResetLightPositionRequested = true;
        }

        void trackBarSpecularStrength_ValueChanged(Object^ sender, EventArgs^ e)
        {
            SpecularStrength = static_cast<float>(trackBarSpecularStrength->Value) / 10.0f;
            labelSpecularStrength->Text = String::Format(
                "Specular Strength                                    {0:0.00}", SpecularStrength);
        }

        void trackBarSpecularColorR_ValueChanged(Object^ sender, EventArgs^ e)
        {
            SpecularColorR = static_cast<float>(trackBarSpecularColorR->Value) / 100.0f;
            labelSpecularColorR->Text = String::Format("Specular Color R                                    {0:0.00}",
                                                       SpecularColorR);
        }

        void trackBarSpecularColorG_ValueChanged(Object^ sender, EventArgs^ e)
        {
            SpecularColorG = static_cast<float>(trackBarSpecularColorG->Value) / 100.0f;
            labelSpecularColorG->Text = String::Format("G                                                  {0:0.00}",
                                                       SpecularColorG);
        }

        void trackBarSpecularColorB_ValueChanged(Object^ sender, EventArgs^ e)
        {
            SpecularColorB = static_cast<float>(trackBarSpecularColorB->Value) / 100.0f;
            labelSpecularColorB->Text = String::Format("B                                                  {0:0.00}",
                                                       SpecularColorB);
        }

        void checkBoxTranslate_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            TranslateEnabled = checkBoxTranslate->Checked;
        }

        void checkBoxRotate_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            RotateEnabled = checkBoxRotate->Checked;
        }

        void checkBoxScale_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            ScaleEnabled = checkBoxScale->Checked;
        }

        void buttonResetTransform_Click(Object^ sender, EventArgs^ e)
        {
            // Reset transform values to default
            checkBoxTranslate->Checked = false;
            checkBoxRotate->Checked = false;
            checkBoxScale->Checked = false;

            TranslateEnabled = false;
            RotateEnabled = false;
            ScaleEnabled = false;
        }

        void trackBarWaterScaleFrequency_ValueChanged(Object^ sender, EventArgs^ e)
        {
            WaterScaleFrequency = static_cast<float>(trackBarWaterScaleFrequency->Value) / 10.0f;
            labelWaterScaleFrequency->Text = String::Format(
                "Frequency                                              {0:0.00}", WaterScaleFrequency);
        }

        void trackBarWaterScaleAmplitude_ValueChanged(Object^ sender, EventArgs^ e)
        {
            WaterScaleAmplitude = static_cast<float>(trackBarWaterScaleAmplitude->Value) / 100.0f;
            labelWaterScaleAmplitude->Text = String::Format(
                "Amplitude                                              {0:0.00}", WaterScaleAmplitude);
        }

        void checkBoxWireframeRender_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            WireframeRenderEnabled = checkBoxWireframeRender->Checked;
        }

        void checkBoxTintBlue_CheckedChanged(Object^ sender, EventArgs^ e)
        {
            TintBlueEnabled = checkBoxTintBlue->Checked;
        }
    };
}
