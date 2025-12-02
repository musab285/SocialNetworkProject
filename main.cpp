#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QMessageBox>
#include "SocialGraph.h"

class SocialNetworkApp : public QWidget {
    // Q_OBJECT // Macro usually needed for signals/slots in header files. 
               // Since we are in main.cpp without MOC preprocessing in some setups, 
               // we will use standard connect functions, but usually, this goes in a .h file.

private:
    SocialGraph sg;

    // UI Elements
    QLineEdit *userEntry;
    QLineEdit *friend1Entry;
    QLineEdit *friend2Entry;
    QLineEdit *recEntry;
    QTextEdit *displayArea;

public:
    SocialNetworkApp(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Social Network Recommendation Engine");
        resize(800, 600);

        // Main Layout
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        QVBoxLayout *leftPanel = new QVBoxLayout();
        QVBoxLayout *rightPanel = new QVBoxLayout();

        // --- LEFT PANEL: CONTROLS ---

        // Group 1: Add User
        QGroupBox *grpAddUser = new QGroupBox("1. Add User");
        QVBoxLayout *layoutUser = new QVBoxLayout();
        userEntry = new QLineEdit();
        userEntry->setPlaceholderText("Enter User Name");
        QPushButton *btnAddUser = new QPushButton("Add User");
        layoutUser->addWidget(userEntry);
        layoutUser->addWidget(btnAddUser);
        grpAddUser->setLayout(layoutUser);

        // Group 2: Create Friendship
        QGroupBox *grpFriends = new QGroupBox("2. Create Connection");
        QVBoxLayout *layoutFriends = new QVBoxLayout();
        friend1Entry = new QLineEdit();
        friend1Entry->setPlaceholderText("User 1 Name");
        friend2Entry = new QLineEdit();
        friend2Entry->setPlaceholderText("User 2 Name");
        QPushButton *btnConnect = new QPushButton("Connect Users");
        layoutFriends->addWidget(friend1Entry);
        layoutFriends->addWidget(friend2Entry);
        layoutFriends->addWidget(btnConnect);
        grpFriends->setLayout(layoutFriends);

        // Group 3: Recommendations
        QGroupBox *grpRec = new QGroupBox("3. Get Recommendations");
        QVBoxLayout *layoutRec = new QVBoxLayout();
        recEntry = new QLineEdit();
        recEntry->setPlaceholderText("Target User Name");
        QPushButton *btnRec = new QPushButton("Find Friends");
        layoutRec->addWidget(recEntry);
        layoutRec->addWidget(btnRec);
        grpRec->setLayout(layoutRec);

        // Group 4: Data Structures View
        QGroupBox *grpDS = new QGroupBox("4. View Data Structures");
        QVBoxLayout *layoutDS = new QVBoxLayout();
        QPushButton *btnShowStack = new QPushButton("Show Recent Stack");
        QPushButton *btnShowQueue = new QPushButton("Show Request Queue");
        layoutDS->addWidget(btnShowStack);
        layoutDS->addWidget(btnShowQueue);
        grpDS->setLayout(layoutDS);

        leftPanel->addWidget(grpAddUser);
        leftPanel->addWidget(grpFriends);
        leftPanel->addWidget(grpRec);
        leftPanel->addWidget(grpDS);
        leftPanel->addStretch();

        // --- RIGHT PANEL: DISPLAY ---
        QLabel *lblDisplay = new QLabel("System Log & Network View:");
        displayArea = new QTextEdit();
        displayArea->setReadOnly(true);
        QPushButton *btnRefresh = new QPushButton("Refresh Network View");

        rightPanel->addWidget(lblDisplay);
        rightPanel->addWidget(displayArea);
        rightPanel->addWidget(btnRefresh);

        // Combine Layouts
        mainLayout->addLayout(leftPanel, 1);
        mainLayout->addLayout(rightPanel, 2);

        // --- BUTTON LOGIC (CONNECTIONS) ---
        
        // Logic: Add User
        connect(btnAddUser, &QPushButton::clicked, [=]() {
            QString name = userEntry->text();
            if(name.isEmpty()) return;
            
            if(sg.addUser(name.toStdString())) {
                log("User Added: " + name);
                userEntry->clear();
                updateNetworkDisplay();
            } else {
                QMessageBox::warning(this, "Error", "User already exists.");
            }
        });

        // Logic: Connect Friends
        connect(btnConnect, &QPushButton::clicked, [=]() {
            QString u1 = friend1Entry->text();
            QString u2 = friend2Entry->text();
            
            if(sg.addFriendship(u1.toStdString(), u2.toStdString())) {
                log("Friendship created: " + u1 + " <--> " + u2);
                friend1Entry->clear();
                friend2Entry->clear();
                updateNetworkDisplay();
            } else {
                QMessageBox::warning(this, "Error", "Failed. Check if users exist or are already friends.");
            }
        });

        // Logic: Recommendations
        connect(btnRec, &QPushButton::clicked, [=]() {
            QString name = recEntry->text();
            string res = sg.recommendFriends(name.toStdString());
            log("\n--- Recommendation Result ---");
            log(QString::fromStdString(res));
        });

        // Logic: Show Stack
        connect(btnShowStack, &QPushButton::clicked, [=]() {
            log("\n--- Recent Stack ---");
            log(QString::fromStdString(sg.getRecentUsers()));
        });

        // Logic: Show Queue
        connect(btnShowQueue, &QPushButton::clicked, [=]() {
            log("\n--- Request Queue ---");
            log(QString::fromStdString(sg.getRecQueue()));
        });

        // Logic: Refresh
        connect(btnRefresh, &QPushButton::clicked, [=]() {
            updateNetworkDisplay();
        });
    }

    void log(QString msg) {
        displayArea->append(msg);
    }

    void updateNetworkDisplay() {
        displayArea->append("\n--- Current Network Topology ---");
        displayArea->append(QString::fromStdString(sg.getNetworkString()));
        displayArea->append("--------------------------------");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    SocialNetworkApp window;
    window.show();

    return app.exec();
}