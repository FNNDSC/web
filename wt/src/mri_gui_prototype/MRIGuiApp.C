//
//
//	Prorotype of MRI GUI App based on one of the examples in Wt
//
//	Author: Dan Ginsburg
//
//  Children's Hospital Boston
//
//	GPL v2
//

#define BOOST_PROCESS_HEADER_ONLY
#include <fstream>

#include <Wt/WApplication>
#include <Wt/WComboBox>
#include <Wt/WSelectionBox>
#include <Wt/WContainerWidget>
#include <Wt/WDatePicker>
#include <Wt/WDateValidator>
#include <Wt/WDialog>
#include <Wt/WImage>
#include <Wt/WIntValidator>
#include <Wt/WItemDelegate>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WGridLayout>
#include <Wt/WPopupMenu>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTreeView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
#include <Wt/WGroupBox>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WHBoxLayout>
#include <Wt/WTextArea>
#include <Wt/WTimer>
#include <Wt/WScrollArea>

#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>


using namespace Wt;
using namespace std;
using namespace boost::processes;

//@TEMP - hackiness to prove that I can do threaded process wait.
//		  This code should of course not be using global variables
//		  and be moved into an appropriate class.
static bool procRunning = false;
static child *procChild = NULL;

void procRunningThread()
{
    procRunning = true;
    status s = procChild->wait();
    procRunning = false;

}
//@TEMP

/// \class MRIGuiApp
/// \brief Main application class.
///
class MRIGuiApp : public WApplication
{
public:
    /// \brief Constructor.
    ///
    MRIGuiApp(const WEnvironment &env)
            : WApplication(env)
    {
        // Create the data models
        mriModel_ = new WStandardItemModel(0, 1, this);

        //@TEMP - these should all be read from a configuration file
        //		  hardcode for now
        dicomDir_ = "/chb/osx1927/1/users/dicom/files/";
        archBaseDir_ = "/home/danginsburg/dan_laptop/arch/Linux";
        scriptBaseDir_ = "/home/danginsburg/dan_laptop/Documents/workspace/trunk/scripts";
        //@TEMP

        populateMRIDs(dicomDir_ + "/dcm_MRID.log");


        mriTocLabel_ = new WLabel("");
        mriScansLabel_ = new WLabel("");

        // Setup the User interface
        createUI();

        // Create timer for polling log
        runTimer_ = new WTimer();
        runTimer_->setInterval(1000);
        runTimer_->timeout().connect(SLOT(this, MRIGuiApp::timeout));

    }

    ///
    ///	Destructor
    ///
    virtual ~MRIGuiApp()
    {
        delete mriTocLabel_;
        delete mriScansLabel_;
        delete runTimer_;
    }

private:

    /// Dicom dir
    std::string dicomDir_;

    /// Script base dir
    std::string scriptBaseDir_;

    /// Arch base dir
    std::string archBaseDir_;

    /// The MRID model (used by mriView_)
    WStandardItemModel    *mriModel_;

    /// List of DICOM files for currently selected scans
    std::vector<std::string> scansDicomFiles_;

    /// The folder view.
    WTreeView *mriView_;

    /// The scans combo box
    WSelectionBox *scansComboBox_;

    /// The contents of the toc.txt file
    WLabel *mriTocLabel_;

    /// The scan info
    WLabel *mriScansLabel_;

    /// Group boxes for holding widgets
    WGroupBox *patientInfoBox_;
    WGroupBox *scanBox_;
    WGroupBox *selectedScanBox_;

    /// Log text area
    WTextArea *logTextArea_;

    /// Radio button (tract)
    WRadioButton *tractButton_;

    /// Radio button (recon)
    WRadioButton *reconButton_;

    /// Execute button
    WPushButton *execButton_;

    /// Processing in progress image
    WImage *progressImage_;

    /// Run timer
    WTimer *runTimer_;


    ///
    /// \brief Setup the user interface.
    ///
    void createUI()
    {
        WContainerWidget *w = root();
        w->setStyleClass("maindiv");

        /// Create primary layout
        WGridLayout *layout = new WGridLayout();
        layout->addWidget(createTitle("MRIDs"), 0, 0, Wt::AlignBottom);
        layout->addWidget(createTitle("Info"), 0, 1, Wt::AlignBottom);
        layout->addWidget(new WImage("icons/chbIntranet.gif"), 0, 2, Wt::AlignRight);

        layout->addWidget(mriView(), 1, 0);

        // Create group boxes
        patientInfoBox_ = new WGroupBox("Patient / Scanner");
        scanBox_ = new WGroupBox("Scans");
        selectedScanBox_ = new WGroupBox("Selected Scan");

        // Set CSS classes
        patientInfoBox_->setStyleClass("testdiv");
        scanBox_->setStyleClass("testdiv");
        selectedScanBox_->setStyleClass("testdiv");

        WVBoxLayout *selectedScanLayout = new WVBoxLayout();
        WText *procText = new WText("Processing Stream:", selectedScanBox_);
        selectedScanLayout->addWidget(procText);
        WButtonGroup *group = new WButtonGroup(selectedScanBox_);
        tractButton_ = new WRadioButton("Tractography", selectedScanBox_);
        reconButton_ = new WRadioButton("Structural reconstruction", selectedScanBox_);
        progressImage_ = new WImage("icons/ajax-loader.gif", selectedScanBox_);

        WHBoxLayout *hBox = new WHBoxLayout();
        execButton_ = new WPushButton("Run", selectedScanBox_);
        execButton_->clicked().connect(SLOT(this, MRIGuiApp::runClicked));
        hBox->addWidget(execButton_);
        hBox->addWidget(progressImage_);

        // logScrollArea_ = new WScrollArea(selectedScanBox_);
        logTextArea_ = new WTextArea("", selectedScanBox_);
        logTextArea_->setStyleClass("logdiv");
        logTextArea_->setRows(15);
        logTextArea_->decorationStyle().font().setFamily(WFont::Monospace);

        logTextArea_->hide();
        hBox->addStretch(200);

        group->addButton(tractButton_);
        group->addButton(reconButton_);
        selectedScanLayout->addWidget(tractButton_);
        selectedScanLayout->addWidget(reconButton_);
        selectedScanLayout->addLayout(hBox);
        selectedScanLayout->addWidget(logTextArea_);
        selectedScanLayout->addStretch(200);
        group->setCheckedButton(group->button(0));

        selectedScanBox_->setLayout(selectedScanLayout);


        patientInfoBox_->addWidget(mriTocLabel_);
        scansComboBox_ = new WSelectionBox(scanBox_);
        scansComboBox_->setVerticalSize(15);
        scansComboBox_->activated().connect(SLOT(this, MRIGuiApp::scanChanged));


        scanBox_->addWidget(scansComboBox_);
        WVBoxLayout *vBox = new WVBoxLayout();
        vBox->addWidget(patientInfoBox_);
        vBox->addWidget(scanBox_);
        layout->addLayout(vBox, 1, 1);

        layout->addWidget(selectedScanBox_, 1, 2);

        patientInfoBox_->hide();
        scanBox_->hide();
        selectedScanBox_->hide();
        progressImage_->hide();

        // Let row 1 and column 1 take the excess space.
        layout->setRowStretch(1, 1);
        layout->setColumnStretch(2, 1);

        w->setLayout(layout);
    }

    /*! \brief Creates a title widget.
     */
    WText *createTitle(const WString& title)
    {
        WText *result = new WText(title);
        result->setInline(false);
        result->setStyleClass("title");

        return result;
    }

    ///
    /// \brief Creates the MRID WTreeView
    ///
    WTreeView *mriView()
    {
        WTreeView *treeView = new WTreeView();

        //
        // To support right-click, we need to disable the built-in browser
        // context menu.
        //
        // Note that disabling the context menu and catching the
        // right-click does not work reliably on all browsers.
        //
        treeView->setAttributeValue
        ("oncontextmenu",
         "event.cancelBubble = true; event.returnValue = false; return false;");
        treeView->setModel(mriModel_);
        treeView->resize(200, WLength::Auto);
        treeView->setSelectionMode(SingleSelection);
        treeView->expandToDepth(1);
        treeView->selectionChanged().connect(SLOT(this,
                                             MRIGuiApp::mriChanged));

        mriView_ = treeView;

        return treeView;
    }


    /// \brief Change the filter on the MRI view when the selected MRI
    ///         changes.
    ///
    void mriChanged()
    {
        if (mriView_->selectedIndexes().empty())
            return;

        WModelIndex selected = *mriView_->selectedIndexes().begin();
        boost::any d = selected.data(UserRole);
        if (!d.empty())
        {
            std::string scanDir = boost::any_cast<std::string>(d);
            stringstream tocURL;

            tocURL << scanDir << "/toc.txt";

            // Parse table-of-contents file
            std::ifstream tocFile(tocURL.str().c_str());
            ostringstream oss;


            scansComboBox_->clear();
            scansDicomFiles_.clear();
            scansComboBox_->setCurrentIndex(0);
            selectedScanBox_->hide();
            if (tocFile.is_open())
            {
                while (!tocFile.eof())
                {
                    char buf[1024];
                    tocFile.getline( buf, sizeof(buf));

                    istringstream istr( string(buf), ios_base::out);
                    string firstToken;
                    istr >> firstToken;

                    if (firstToken == "Scan")
                    {
                        string dicomFile,
                        scanName;

                        istr >> dicomFile;
                        while (!istr.eof())
                        {
                            string tmp;

                            istr >> tmp;
                            scanName += tmp + " ";
                        }

                        scansComboBox_->addItem(scanName);
                        scansDicomFiles_.push_back(dicomFile);

                    }
                    else
                    {
                        oss << string(buf) << "<br/>";
                    }
                }

                mriTocLabel_->setText(oss.str());

                tocFile.close();
                scanBox_->show();
                patientInfoBox_->show();
            }
        }
    }

    /// \brief Change the filter on the file view when the selected folder
    ///         changes.
    void scanChanged()
    {
        if (scansComboBox_->currentIndex() >= 0)
        {
            selectedScanBox_->show();
        }
    }

    ///
    /// \brief Create an MRI item.
    ///
    WStandardItem *createMRIItem(const std::string& MRID,
                                 const std::string& scanDir)
    {
        WStandardItem *result = new WStandardItem(MRID);

        result->setData(scanDir);
        result->setIcon("icons/folder.gif");

        return result;
    }



    ///
    /// \brief Populate the MRIDs model.
    ///
    void populateMRIDs(const std::string& mridLogFile)
    {
        std::ifstream inFile(mridLogFile.c_str());
        while (!inFile.eof())
        {
            string scanName,
            MRID;

            char buf[1024] = {0};
            inFile.getline( buf, sizeof(buf));

            istringstream istr( string(buf), ios_base::out);
            istr >> scanName >> MRID;

            stringstream scanDir;
            scanDir << dicomDir_ << "/" << scanName;

            stringstream tocURL;

            tocURL << scanDir.str() << "/toc.txt";
            cout << tocURL.str() << endl;

            std::ifstream tocFile(tocURL.str().c_str());
            if (tocFile.is_open())
            {
                mriModel_->appendRow(createMRIItem(MRID, scanDir.str()));

            }

        }
    }

    ///
    ///	\brief Run button clicked, execute fs_meta
    ///
    void runClicked()
    {
        stringstream cmdToExecute;

        if (mriView_->selectedIndexes().empty())
            return;

        if (scansComboBox_->currentIndex() < 0)
            return;

        execButton_->disable();
        progressImage_->show();


        WModelIndex selected = *mriView_->selectedIndexes().begin();
        boost::any d = selected.data(UserRole);
        if (!d.empty())
        {
            std::string scanDir = boost::any_cast<std::string>(d);
            logTextArea_->show();

            //@TEMP - temporarily hard-coded paths.  These should be read from a configuration file
            //	      or some other method, should not require recompilation
            cmdToExecute << scriptBaseDir_ << "/fs_meta_web.bash " <<
            archBaseDir_ << "/packages " <<
            scriptBaseDir_  <<
            " \"-D " << scanDir <<
            " -d " << scansDicomFiles_[scansComboBox_->currentIndex()] <<
            " -v 10 -f\"";

            // Execute in a separate process using boost
            context ctx;
            child c = launch_shell(cmdToExecute.str().c_str(), ctx);
            procChild = new child(c);
            boost::thread waitThread(&procRunningThread);

            logTextArea_->setText(cmdToExecute.str());

            runTimer_->start();
        }

    }

    ///
    ///	\brief Timer timeout to scan log
    ///
    void timeout()
    {
        if (!procRunning)
        {
            execButton_->enable();
            progressImage_->hide();
            runTimer_->stop();
        }

        //@TEMP - log file would need to be in specific location, this was
        //		  done as a test
        std::ifstream logFile("/home/danginsburg/fs_meta.bash.std", ios::in);
        if (logFile.is_open())
        {
            ostringstream oss;

            oss << logFile.rdbuf();

            stringstream logText;
            logText << oss.str();
            logTextArea_->setText(logText.str().c_str());
            logFile.close();

            // First, take the lock to safely manipulate the UI outside of the
            // normal event loop, by having exclusive access to the session.
            WApplication::UpdateLock lock = getUpdateLock();

            //
            // Little javascript trick to make sure we scroll along with new content
            //
            doJavaScript(logTextArea_->jsRef() + ".scrollTop += "
                         + logTextArea_->jsRef() + ".scrollHeight;");

            triggerUpdate();


        }
        else
        {
            logTextArea_->setText("Couldn't open log!");
        }
    }

};

WApplication *createApplication(const WEnvironment& env)
{
    WApplication *app = new MRIGuiApp(env);
    app->setTwoPhaseRenderingThreshold(0);
    app->setTitle("Children's Hospital Boston Imaging Pipeline");
    app->useStyleSheet("styles.css");
    app->messageResourceBundle().use("about");
    app->refresh();



    return app;
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &createApplication);
}

/*@}*/
