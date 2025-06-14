#include "ViewLayer.h"


ViewLayer::ViewLayer(QWidget *parent) {

}

void ViewLayer::init() {

}

bool ViewLayer::parseTime(const std::string &str, Time &result) {
    return false;
}

void ViewLayer::showView(QWidget *view) {

}

bool ViewLayer::parseDate(const std::string &str, Date &result) {
    return false;
}

void ViewLayer::clearLayout(QLayout *layout) {

}

void ViewLayer::initEventManageView() {

}

void ViewLayer::initHabitManageView() {

}

void ViewLayer::initNavigationView() {

}

void ViewLayer::onBackToNavigation() {

}

void ViewLayer::onDeleteEventClicked() {

}

void ViewLayer::onAddEventClicked() {

}

void ViewLayer::onDeleteHabitClicked() {

}

void ViewLayer::onAddHabitClicked() {

}

void ViewLayer::eventDeleted(const Event &event) {

}

void ViewLayer::eventAdded() {

}

void ViewLayer::habitDeleted(const Habit &habit) {

}

void ViewLayer::habitAdded() {

}

void ViewLayer::setcurrentView(ViewType view) {

}
