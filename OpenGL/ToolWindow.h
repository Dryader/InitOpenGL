#pragma once

namespace OpenGL {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ToolWindow
	/// </summary>
	public ref class ToolWindow : public System::Windows::Forms::Form
	{
	public:
		static bool RenderRedChannel;
		static bool RenderGreenChannel;
		static bool RenderBlueChannel;


		ToolWindow(void)
		{
			InitializeComponent();
			RenderRedChannel = checkBoxRedChannel->Checked;
			RenderGreenChannel = checkBoxGreenChannel->Checked;
			RenderBlueChannel = checkBoxBlueChannel->Checked;
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
	private: System::Windows::Forms::CheckBox^ checkBoxRedChannel;
	private: System::Windows::Forms::CheckBox^ checkBoxGreenChannel;
	private: System::Windows::Forms::CheckBox^ checkBoxBlueChannel;




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->checkBoxRedChannel = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxGreenChannel = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxBlueChannel = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// checkBoxRedChannel
			// 
			this->checkBoxRedChannel->AutoSize = true;
			this->checkBoxRedChannel->Location = System::Drawing::Point(13, 13);
			this->checkBoxRedChannel->Name = L"checkBoxRedChannel";
			this->checkBoxRedChannel->Size = System::Drawing::Size(107, 20);
			this->checkBoxRedChannel->TabIndex = 0;
			this->checkBoxRedChannel->Text = L"Red Channel";
			this->checkBoxRedChannel->UseVisualStyleBackColor = true;
			this->checkBoxRedChannel->CheckedChanged += gcnew System::EventHandler(this, &ToolWindow::checkBoxRedChannel_CheckedChanged);
			// 
			// checkBoxGreenChannel
			// 
			this->checkBoxGreenChannel->AutoSize = true;
			this->checkBoxGreenChannel->Location = System::Drawing::Point(13, 40);
			this->checkBoxGreenChannel->Name = L"checkBoxGreenChannel";
			this->checkBoxGreenChannel->Size = System::Drawing::Size(118, 20);
			this->checkBoxGreenChannel->TabIndex = 1;
			this->checkBoxGreenChannel->Text = L"Green Channel";
			this->checkBoxGreenChannel->UseVisualStyleBackColor = true;
			this->checkBoxGreenChannel->CheckedChanged += gcnew System::EventHandler(this, &ToolWindow::checkBoxGreenChannel_CheckedChanged);
			// 
			// checkBoxBlueChannel
			// 
			this->checkBoxBlueChannel->AutoSize = true;
			this->checkBoxBlueChannel->Location = System::Drawing::Point(13, 67);
			this->checkBoxBlueChannel->Name = L"checkBoxBlueChannel";
			this->checkBoxBlueChannel->Size = System::Drawing::Size(108, 20);
			this->checkBoxBlueChannel->TabIndex = 2;
			this->checkBoxBlueChannel->Text = L"Blue Channel";
			this->checkBoxBlueChannel->UseVisualStyleBackColor = true;
			this->checkBoxBlueChannel->CheckedChanged += gcnew System::EventHandler(this, &ToolWindow::checkBoxBlueChannel_CheckedChanged);
			// 
			// ToolWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(282, 253);
			this->Controls->Add(this->checkBoxBlueChannel);
			this->Controls->Add(this->checkBoxGreenChannel);
			this->Controls->Add(this->checkBoxRedChannel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"ToolWindow";
			this->Text = L"ToolWindow";
			this->TopMost = true;
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void checkBoxRedChannel_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		RenderRedChannel = checkBoxRedChannel->Checked;
	}
	private: System::Void checkBoxGreenChannel_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		RenderGreenChannel = checkBoxGreenChannel->Checked;
	}
	private: System::Void checkBoxBlueChannel_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		RenderBlueChannel = checkBoxBlueChannel->Checked;
	}
	};
}