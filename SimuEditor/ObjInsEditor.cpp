#include "ObjInsEditor.h"
#include "MainForm.h"
#include "DsSimu.h"
#include < vcclr.h >

using namespace SimuEditor;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace DsApp;

void ObjInsEditor::EditObjInsData(ListViewItem^ item)
{
	ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
	if (data){
		UpdateListViewItemParam(item);
		ObjInsNameTextBox->Text = data->objInsName;	//���f�[�^��������C���X�^���X��
		drawModelPathTextBox->Text = data->drawModelPath;	//���f�[�^�p�X
		hitModelPathTextBox->Text = data->hitModelPath;	//���f�[�^�p�X
		PosX->Value = static_cast<Decimal>(data->posX);
		PosX->Value = static_cast<Decimal>(data->posX);
		PosY->Value = static_cast<Decimal>(data->posY);
		PosZ->Value = static_cast<Decimal>(data->posZ);
		AngX->Value = static_cast<Decimal>(data->angX);
		AngY->Value = static_cast<Decimal>(data->angY);
		AngZ->Value = static_cast<Decimal>(data->angZ);

		if (data->objType == 0)
		{
			ObjTypeComboBox->Text = L"�ÓI�I�u�W�F";
		}
		else if (data->objType == 1)
		{
			ObjTypeComboBox->Text = L"���I�I�u�W�F";
		}
		else if (data->objType == 2)
		{
			ObjTypeComboBox->Text = L"�����I�u�W�F";
		}
		else if (data->objType == 3)
		{
			ObjTypeComboBox->Text = L"����I�u�W�F";
		}

		if (data->fieldObjType == 0)
		{
			FieldObjComboBox->Text = L"�L����";
		}
		else if (data->fieldObjType == 1)
		{
			FieldObjComboBox->Text = L"�I�u�W�F";
		}
		else if (data->fieldObjType == 2)
		{
			FieldObjComboBox->Text = L"�q�b�g";
		}
		else if (data->fieldObjType == 3)
		{
			FieldObjComboBox->Text = L"�v���C���[";
		}

		m_editItem = item;

	}else{
		MessageBox::Show(L"EditObjInsData�̈�����ObjIns�̃A�C�e���^�O�ł͂���܂���B");
	}

}

//�I�u�W�F�C���X�Ƀf�[�^�Z�b�g
ListViewItem^ ObjInsEditor::_SetObjInsData()
{
	//�����p�����[�^�ł͊����{�^���������Ȃ�
	if ((ObjInsNameTextBox->Text->Length <= 0) || (hitModelPathTextBox->Text->Length <= 0) ||
		(drawModelPathTextBox->Text->Length <= 0))
	{
		return nullptr;
	}

	MainForm^ form = static_cast<MainForm^>(this->Owner);
	if (form)
	{
		ObjInsFormat^ data = (m_editItem) ? (static_cast<ObjInsFormat^>(m_editItem->Tag)) : (gcnew ObjInsFormat());//�ҏW���[�h�̂Ƃ��͌��ݒl�R�s�[��
		data->objInsName = ObjInsNameTextBox->Text;	//���f�[�^��������C���X�^���X��
		data->drawModelPath = drawModelPathTextBox->Text;	//���f�[�^�p�X
		data->hitModelPath = hitModelPathTextBox->Text;	//���f�[�^�p�X
		data->posX = Decimal::ToDouble(PosX->Value);
		data->posY = Decimal::ToDouble(PosY->Value);
		data->posZ = Decimal::ToDouble(PosZ->Value);
		data->angX = Decimal::ToDouble(AngX->Value);
		data->angY = Decimal::ToDouble(AngY->Value);
		data->angZ = Decimal::ToDouble(AngZ->Value);
		if (ObjTypeComboBox->Text == L"�ÓI�I�u�W�F")
		{
			data->objType = 0;
		}
		else if (ObjTypeComboBox->Text == L"���I�I�u�W�F")
		{
			data->objType = 1;
		}
		else if (ObjTypeComboBox->Text == L"�����I�u�W�F")
		{
			data->objType = 2;
		}
		else if (ObjTypeComboBox->Text == L"����I�u�W�F")
		{
			data->objType = 3;
		}

		if (FieldObjComboBox->Text == L"�L����")
		{
			data->fieldObjType = 0;
		}
		else if (FieldObjComboBox->Text == L"�I�u�W�F")
		{
			data->fieldObjType = 1;
		}
		else if (FieldObjComboBox->Text == L"�q�b�g")
		{
			data->fieldObjType = 2;
		}
		else if (FieldObjComboBox->Text == L"�v���C���[")
		{
			data->fieldObjType = 3;
		}

		if (m_editItem)
		{
			//�ҏW���ʂ�ۑ�
			m_editItem->Text = data->objInsName;
			m_editItem->Tag = data;
			return m_editItem;
		}
		else
		{
			//�ǉ��B
			for each(ListViewItem^ item in form->ObjInsListView->Items)
			{
				if (data->objInsName == item->Text)
				{
					//���ɂ������̂Œ��~
					MessageBox::Show(data->objInsName + L"�͊��ɑ��݂��܂�");
					return nullptr;
				}
			}

			ListViewItem^ newItem = gcnew ListViewItem();
			newItem->Text = data->objInsName;
			newItem->Tag = data;
			form->ObjInsListView->Items->Add(newItem);
			return newItem;
		}
		
	}
	return nullptr;
}

//�I�u�W�F�C���X���ɃL�[����
System::Void ObjInsEditor::ObjInsNameTextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::Enter)
	{
		Complete_Click(sender, e);
	}
}

//�����{�^��
System::Void ObjInsEditor::Complete_Click(System::Object^  sender, System::EventArgs^  e)
{
	ListViewItem^ item = _SetObjInsData();
	if (nullptr!=item)
	{
		//�ҏW����Ȃ��Ƃ��̂݃I�u�W�F�쐬�B�ҏW�̂Ƃ��쐬���Ă��܂��Ɠ������̂������ł���
		if (m_editItem == nullptr)
		{
			char* drawModelPath = CreateCharArray(drawModelPathTextBox->Text);
			char* hitModelPath = CreateCharArray(hitModelPathTextBox->Text);
			ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);

			data->pFieldObj =
			DsSimu::GetIns()->RegisterObj(drawModelPath, hitModelPath,
				Decimal::ToDouble(PosX->Value), Decimal::ToDouble(PosY->Value), Decimal::ToDouble(PosZ->Value),
				Decimal::ToDouble(AngX->Value), Decimal::ToDouble(AngY->Value), Decimal::ToDouble(AngZ->Value),
				static_cast<DS_MAP_OBJ_TYPE>(data->objType),
				static_cast<DS_MAP_FIELD_OBJ_TYPE>(data->fieldObjType));
			delete drawModelPath;
			delete hitModelPath;

			item->Tag = data;
		}
		else //�ҏW�̂Ƃ���FieldObj�ɔ��f������
		{
			ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
			DsFieldObj* pFieldObj = static_cast<DsFieldObj*>(data->pFieldObj);
			const DsMat33d rot = DsMat33d::RotateX(DegToRad(data->angX))*DsMat33d::RotateY(DegToRad(data->angY))*DsMat33d::RotateZ(DegToRad(data->angZ));
			pFieldObj->SetPosition(DsVec3d(data->posX, data->posY, data->posZ));
			pFieldObj->SetRotation(rot);
			
		}
		this->Close();
	}
}

//�f�[�^�p�X�J���{�^��
System::Void ObjInsEditor::OpenFileDrawModelPathButton_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog^ openDialog = gcnew OpenFileDialog();
	openDialog->Filter = L"�Ǝ��`���̃��f�����|*.dmdl";
	if (openDialog->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		drawModelPathTextBox->Text = openDialog->FileName;
		
		//�f�[�^�̖��O�ɏ]���ăf�t�H���g�l��\�����đ��
		if (openDialog->FileName->Contains(L"hit") || openDialog->FileName->Contains(L"map"))
		{
			ObjTypeComboBox->Text = L"�ÓI�I�u�W�F";
			FieldObjComboBox->Text = L"�q�b�g";
		}
		else if (openDialog->FileName->Contains(L"obj"))
		{
			ObjTypeComboBox->Text = L"���I�I�u�W�F";
			FieldObjComboBox->Text = L"�I�u�W�F";
		}
		else if (openDialog->FileName->Contains(L"chr"))
		{
			ObjTypeComboBox->Text = L"�����I�u�W�F";
			FieldObjComboBox->Text = L"�L����";
		}

		//�f�[�^����obj�������߂�
		MainForm^ form = static_cast<MainForm^>(this->Owner);
		if (form)
		{
			//�d���`�F�b�N
			String^ fileName = System::IO::Path::GetFileNameWithoutExtension(openDialog->FileName);
			int sameCount = 0;
			for each(ListViewItem^ item in form->ObjInsListView->Items)
			{
				ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
				String^ alreadyPath = System::IO::Path::GetFileNameWithoutExtension(data->drawModelPath);
				if (alreadyPath == fileName)
				{
					++sameCount;
				}
			}

			if (0 < sameCount)
			{
				fileName = fileName + sameCount;
			}
			ObjInsNameTextBox->Text = fileName;
		}
	}
}

// �f�[�^�p�X�J���{�^��
System::Void ObjInsEditor::OpenFileHitModelPathButton_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog^ openDialog = gcnew OpenFileDialog();
	openDialog->Filter = L"�Ǝ��`���̃��f�����|*.dmdl;*.txt";
	if (openDialog->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		hitModelPathTextBox->Text = openDialog->FileName;
	}
}