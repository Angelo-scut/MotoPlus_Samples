#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test_ui.h"
#include "yrc_control.hpp"
#include <vector>
#include <memory>
#include <qtimer.h>

class test_ui : public QMainWindow
{
    Q_OBJECT

public:
    test_ui(QWidget *parent = nullptr);
    ~test_ui();

	void init_widget(bool state=false);

private:
	void connect_button_event();

	void x_spnbx_valchg_event();
	void y_spnbx_valchg_event();
	void z_spnbx_valchg_event();
	void rx_spnbx_valchg_event();
	void ry_spnbx_valchg_event();
	void rz_spnbx_valchg_event();
	void spd_spnbx_valchg_event();
	//void corrPSA_spnbx_valchg_event();

	void line_manual_button_event();
	void line_pc_button_event();
	void fast_loc_manual_button_event();
	void fast_loc_pc_button_event();
	void circle_manual_button_event();
	void circle_pc_button_event();
	void get_position_button_event();
	void get_speed_button_event();
	void corrpath_position_button_event();
	void corrpath_axis_button_event();
	void force_endpath_button_event();
	void change_speed_button_event();
	void restore_speed_button_event();

	void position_update_event();

	void update_statusBar();
	std::string position_encoding();

	QTimer* status_update_timer;
	float spd_fct = 0.0f;
	float spd = 0.0f;
	std::vector<float> pos;
	std::vector<float> pos_corr;
	std::shared_ptr<yrc::YRC_control> controller;

    Ui::test_uiClass ui;
};
