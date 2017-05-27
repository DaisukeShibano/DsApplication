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
		ObjInsNameTextBox->Text = data->objInsName;	//元データから作られるインスタンス名
		drawModelPathTextBox->Text = data->drawModelPath;	//元データパス
		hitModelPathTextBox->Text = data->hitModelPath;	//元データパス
		PosX->Value = static_cast<Decimal>(data->posX);
		PosX->Value = static_cast<Decimal>(data->posX);
		PosY->Value = static_cast<Decimal>(data->posY);
		PosZ->Value = static_cast<Decimal>(data->posZ);
		AngX->Value = static_cast<Decimal>(data->angX);
		AngY->Value = static_cast<Decimal>(data->angY);
		AngZ->Value = static_cast<Decimal>(data->angZ);

		if (data->objType == 0)
		{
			ObjTypeComboBox->Text = L"静的オブジェ";
		}
		else if (data->objType == 1)
		{
			ObjTypeComboBox->Text = L"動的オブジェ";
		}
		else if (data->objType == 2)
		{
			ObjTypeComboBox->Text = L"自律オブジェ";
		}
		else if (data->objType == 3)
		{
			ObjTypeComboBox->Text = L"操作オブジェ";
		}

		if (data->fieldObjType == 0)
		{
			FieldObjComboBox->Text = L"キャラ";
		}
		else if (data->fieldObjType == 1)
		{
			FieldObjComboBox->Text = L"オブジェ";
		}
		else if (data->fieldObjType == 2)
		{
			FieldObjComboBox->Text = L"ヒット";
		}
		else if (data->fieldObjType == 3)
		{
			FieldObjComboBox->Text = L"プレイヤー";
		}

		m_editItem = item;

	}else{
		MessageBox::Show(L"EditObjInsDataの引数がObjInsのアイテムタグではありません。");
	}

}

//オブジェインスにデータセット
ListViewItem^ ObjInsEditor::_SetObjInsData()
{
	//無効パラメータでは完了ボタン押させない
	if ((ObjInsNameTextBox->Text->Length <= 0) || (hitModelPathTextBox->Text->Length <= 0) ||
		(drawModelPathTextBox->Text->Length <= 0))
	{
		return nullptr;
	}

	MainForm^ form = static_cast<MainForm^>(this->Owner);
	if (form)
	{
		ObjInsFormat^ data = (m_editItem) ? (static_cast<ObjInsFormat^>(m_editItem->Tag)) : (gcnew ObjInsFormat());//編集モードのときは現在値コピーで
		data->objInsName = ObjInsNameTextBox->Text;	//元データから作られるインスタンス名
		data->drawModelPath = drawModelPathTextBox->Text;	//元データパス
		data->hitModelPath = hitModelPathTextBox->Text;	//元データパス
		data->posX = Decimal::ToDouble(PosX->Value);
		data->posY = Decimal::ToDouble(PosY->Value);
		data->posZ = Decimal::ToDouble(PosZ->Value);
		data->angX = Decimal::ToDouble(AngX->Value);
		data->angY = Decimal::ToDouble(AngY->Value);
		data->angZ = Decimal::ToDouble(AngZ->Value);
		if (ObjTypeComboBox->Text == L"静的オブジェ")
		{
			data->objType = 0;
		}
		else if (ObjTypeComboBox->Text == L"動的オブジェ")
		{
			data->objType = 1;
		}
		else if (ObjTypeComboBox->Text == L"自律オブジェ")
		{
			data->objType = 2;
		}
		else if (ObjTypeComboBox->Text == L"操作オブジェ")
		{
			data->objType = 3;
		}

		if (FieldObjComboBox->Text == L"キャラ")
		{
			data->fieldObjType = 0;
		}
		else if (FieldObjComboBox->Text == L"オブジェ")
		{
			data->fieldObjType = 1;
		}
		else if (FieldObjComboBox->Text == L"ヒット")
		{
			data->fieldObjType = 2;
		}
		else if (FieldObjComboBox->Text == L"プレイヤー")
		{
			data->fieldObjType = 3;
		}

		if (m_editItem)
		{
			//編集結果を保存
			m_editItem->Text = data->objInsName;
			m_editItem->Tag = data;
			return m_editItem;
		}
		else
		{
			//追加。
			for each(ListViewItem^ item in form->ObjInsListView->Items)
			{
				if (data->objInsName == item->Text)
				{
					//既にあったので中止
					MessageBox::Show(data->objInsName + L"は既に存在します");
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

//オブジェインス名にキー入力
System::Void ObjInsEditor::ObjInsNameTextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::Enter)
	{
		Complete_Click(sender, e);
	}
}

//完了ボタン
System::Void ObjInsEditor::Complete_Click(System::Object^  sender, System::EventArgs^  e)
{
	ListViewItem^ item = _SetObjInsData();
	if (nullptr!=item)
	{
		//編集じゃないときのみオブジェ作成。編集のとき作成してしまうと同じものが複数できる
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
		else //編集のときはFieldObjに反映させる
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

//データパス開くボタン
System::Void ObjInsEditor::OpenFileDrawModelPathButton_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog^ openDialog = gcnew OpenFileDialog();
	openDialog->Filter = L"独自形式のモデル情報|*.dmdl";
	if (openDialog->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		drawModelPathTextBox->Text = openDialog->FileName;
		
		//データの名前に従ってデフォルト値を予測して代入
		if (openDialog->FileName->Contains(L"hit") || openDialog->FileName->Contains(L"map"))
		{
			ObjTypeComboBox->Text = L"静的オブジェ";
			FieldObjComboBox->Text = L"ヒット";
		}
		else if (openDialog->FileName->Contains(L"obj"))
		{
			ObjTypeComboBox->Text = L"動的オブジェ";
			FieldObjComboBox->Text = L"オブジェ";
		}
		else if (openDialog->FileName->Contains(L"chr"))
		{
			ObjTypeComboBox->Text = L"自律オブジェ";
			FieldObjComboBox->Text = L"キャラ";
		}

		//データ名でobj名を決める
		MainForm^ form = static_cast<MainForm^>(this->Owner);
		if (form)
		{
			//重複チェック
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

// データパス開くボタン
System::Void ObjInsEditor::OpenFileHitModelPathButton_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog^ openDialog = gcnew OpenFileDialog();
	openDialog->Filter = L"独自形式のモデル情報|*.dmdl;*.txt";
	if (openDialog->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		hitModelPathTextBox->Text = openDialog->FileName;
	}
}