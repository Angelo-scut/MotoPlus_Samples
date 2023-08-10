#include "test_ui.h"
#include <qspinbox.h>
#include <qspinbox.h>
#include <string>

using namespace std;
using namespace yrc;
test_ui::test_ui(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	init_widget();

	pos = std::vector<float>(6, 0);
	//pos_corr = std::vector<float>(6, 0);
	status_update_timer = new QTimer();
	controller = make_shared<YRC_control>();
	ui.ipEdit->setText("192.168.255.1");
	ui.portEdit->setText("11000");

	QObject::connect(status_update_timer, &QTimer::timeout, this, &test_ui::update_statusBar);

	QObject::connect(ui.xSpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::x_spnbx_valchg_event);
	QObject::connect(ui.ySpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::y_spnbx_valchg_event);
	QObject::connect(ui.zSpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::z_spnbx_valchg_event);
	QObject::connect(ui.rxSpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::rx_spnbx_valchg_event);
	QObject::connect(ui.rySpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::ry_spnbx_valchg_event);
	QObject::connect(ui.rzSpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::rz_spnbx_valchg_event);

	QObject::connect(ui.spdSpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::spd_spnbx_valchg_event);
	//QObject::connect(ui.corrPSASpnBx, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &test_ui::corrPSA_spnbx_valchg_event);

	QObject::connect(ui.changeSpdBtn, &QPushButton::clicked, this, &test_ui::change_speed_button_event);
	QObject::connect(ui.circlePCBtn, &QPushButton::clicked, this, &test_ui::circle_pc_button_event);
	QObject::connect(ui.circleMnlBtn, &QPushButton::clicked, this, &test_ui::circle_manual_button_event);
	QObject::connect(ui.connectBtn, &QPushButton::clicked, this, &test_ui::connect_button_event);
	QObject::connect(ui.corrPathAxisBtn, &QPushButton::clicked, this, &test_ui::corrpath_axis_button_event);
	QObject::connect(ui.corrPathPosBtn, &QPushButton::clicked, this, &test_ui::corrpath_position_button_event);
	QObject::connect(ui.fastLocMnlBtn, &QPushButton::clicked, this, &test_ui::fast_loc_manual_button_event);
	QObject::connect(ui.fastLPCBtn, &QPushButton::clicked, this, &test_ui::fast_loc_pc_button_event);
	QObject::connect(ui.forceEndPathBtn, &QPushButton::clicked, this, &test_ui::force_endpath_button_event);
	QObject::connect(ui.getPosBtn, &QPushButton::clicked, this, &test_ui::get_position_button_event);
	QObject::connect(ui.getSpdBtn, &QPushButton::clicked, this, &test_ui::get_speed_button_event);
	QObject::connect(ui.lineMnlBtn, &QPushButton::clicked, this, &test_ui::line_manual_button_event);
	QObject::connect(ui.linePCBtn, &QPushButton::clicked, this, &test_ui::line_pc_button_event);
	QObject::connect(ui.restoreSpdBtn, &QPushButton::clicked, this, &test_ui::restore_speed_button_event);
	//update_statusBar();
}

test_ui::~test_ui()
{
	pos.clear();
	pos_corr.clear();
}

void test_ui::init_widget(bool state)
{
	ui.changeSpdBtn->setEnabled(state);
	ui.circlePCBtn->setEnabled(state);
	ui.circleMnlBtn->setEnabled(state);
	ui.corrPathPosBtn->setEnabled(state);
	ui.fastLocMnlBtn->setEnabled(state);
	ui.fastLPCBtn->setEnabled(state);
	ui.getPosBtn->setEnabled(state);
	ui.getSpdBtn->setEnabled(state);
	ui.linePCBtn->setEnabled(state);
	ui.corrPathAxisBtn->setEnabled(state);
	ui.lineMnlBtn->setEnabled(state);
	ui.forceEndPathBtn->setEnabled(state);
	ui.restoreSpdBtn->setEnabled(state);
}

void test_ui::connect_button_event()
{
	string ip = ui.ipEdit->text().toStdString();
	int port = ui.portEdit->text().toInt();
	controller->connect(ip, port);
	init_widget(true);
	get_position_button_event();
	get_speed_button_event();
	//status_update_timer->start(100);
}

void test_ui::x_spnbx_valchg_event()
{
	pos[0] = ui.xSpnBx->value();
}

void test_ui::y_spnbx_valchg_event()
{
	pos[1] = ui.ySpnBx->value();
}

void test_ui::z_spnbx_valchg_event()
{
	pos[2] = ui.zSpnBx->value();
}

void test_ui::rx_spnbx_valchg_event()
{
	pos[3] = ui.rxSpnBx->value();
}

void test_ui::ry_spnbx_valchg_event()
{
	pos[4] = ui.rySpnBx->value();
}

void test_ui::rz_spnbx_valchg_event()
{
	pos[5] = ui.rzSpnBx->value();
}

void test_ui::spd_spnbx_valchg_event()
{
	spd_fct = ui.spdSpnBx->value();
}

//void test_ui::corrPSA_spnbx_valchg_event()
//{
//	pos_corr = std::vector<float>(6, 0.f);
//	QString cmd = ui.corrPSACmbBx->currentText();
//	if (cmd == "x")			pos_corr[0] = ui.corrPSASpnBx->value();
//	else if (cmd == "y")	pos_corr[1] = ui.corrPSASpnBx->value();
//	else if (cmd == "z")	pos_corr[2] = ui.corrPSASpnBx->value();
//	else if (cmd == "rx")	pos_corr[3] = ui.corrPSASpnBx->value();
//	else if (cmd == "ry")	pos_corr[4] = ui.corrPSASpnBx->value();
//	else if (cmd == "rz")	pos_corr[5] = ui.corrPSASpnBx->value();
//}

void test_ui::line_manual_button_event()
{
	controller->line_manual();
}

void test_ui::line_pc_button_event()
{
	controller->line_pc(pos);
}

void test_ui::fast_loc_manual_button_event()
{
	controller->fast_locate_manual();
}

void test_ui::fast_loc_pc_button_event()
{
	controller->fast_locate_pc(pos);
}

void test_ui::circle_manual_button_event()
{
	controller->circle_manual();
}

void test_ui::circle_pc_button_event()
{
	//controller->circle_pc();
}

void test_ui::get_position_button_event()
{
	pos = controller->get_position();
	update_statusBar();
	position_update_event();
	//TODO: update position information to the status bar
}

void test_ui::get_speed_button_event()
{
	spd = controller->get_speed();
	update_statusBar();
}

void test_ui::corrpath_position_button_event()
{
	//controller->corr_path(pos);  // 不能直接纠偏，因为本来就应该在1mm之内的，这里没有加限制
}

void test_ui::corrpath_axis_button_event()
{
	QString cmd = ui.corrPSACmbBx->currentText();
	if (cmd == "x")			controller->corr_path(ui.corrPSASpnBx->value(), 0);
	else if (cmd == "y")	controller->corr_path(ui.corrPSASpnBx->value(), 1);
	else if (cmd == "z")	controller->corr_path(ui.corrPSASpnBx->value(), 2);
	else if (cmd == "rx")	controller->corr_path(ui.corrPSASpnBx->value(), 3);
	else if (cmd == "ry")	controller->corr_path(ui.corrPSASpnBx->value(), 4);
	else if (cmd == "rz")	controller->corr_path(ui.corrPSASpnBx->value(), 5);
}

void test_ui::force_endpath_button_event()
{
	controller->force_path_end();
}

void test_ui::change_speed_button_event()
{
	controller->change_speed(spd_fct);
}

void test_ui::restore_speed_button_event()
{
	controller->restore_speed();
}

void test_ui::position_update_event()
{
	if (pos.size() < 6) return;
	ui.xSpnBx->setValue(pos[0]);
	ui.ySpnBx->setValue(pos[1]);
	ui.zSpnBx->setValue(pos[2]);
	ui.rxSpnBx->setValue(pos[3]);
	ui.rySpnBx->setValue(pos[4]);
	ui.rzSpnBx->setValue(pos[5]);
}

void test_ui::update_statusBar()
{
	//get_position_button_event();
	//get_speed_button_event();
	QString msg = QString::fromStdString(position_encoding() + "   SPEED:" + to_string(spd));
	ui.statusBar->showMessage(msg);
}

std::string test_ui::position_encoding()
{
	string pos_msg = "POSITION:(";

	for (size_t i = 0; i < pos.size(); i++)
	{
		pos_msg += to_string(pos[i]);
		if (i != pos.size() - 1)
			pos_msg += ",";

	}
	pos_msg += ")";
	return pos_msg;
}
