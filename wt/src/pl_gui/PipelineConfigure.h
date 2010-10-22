//
//
//  Description:
//      This is the container widget for the pipeline configuration
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINECONFIGURE_H
#define PIPELINECONFIGURE_H

#include <Wt/WContainerWidget>
#include <vector>
#include <string>
#include "GlobalEnums.h"
#include "ScanBrowser.h"
#include "PipelineStatus.h"

class PipelineOptions;
class PipelineOptionsTract;
class PipelineOptionsFS;
class PipelineOptionsFetal;
class PipelineOptionsDcmSend;
class PipelineOptionsConnectome;
namespace Wt
{
    class WStackedWidget;
    class WButtonGroup;
}

using namespace Wt;

///
/// \class PipelineConfigure
/// \brief Container widget for the contents of the pipeline configuration stage
///
class PipelineConfigure : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    PipelineConfigure(WContainerWidget *parent=NULL);

    ///
    /// Destructor
    ///
    virtual ~PipelineConfigure();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    ///  Update all elements of widget to current values (on next clicked)
    ///
    void updateAll(SelectScans *selectScans);

    ///
    /// Check whether the options are valid
    ///
    bool validate() const;

    ///
    ///  Generate command-line options string based on user choices
    ///
    virtual std::string getCommandLineString() const;

    ///
    ///  Get the current directory suffix
    ///
    virtual std::string getOutputDirSuffix() const;

    ///
    ///  Get the current file suffix
    ///
    virtual std::string getOutputFileSuffix() const;

private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);


private:

    /// Pipeline status
    PipelineStatus *mPipelineStatus;

    /// Stacked widget for pipeline options box
    WStackedWidget *mStackedPipelineOptions;

    /// Tractography pipeline options
    PipelineOptionsTract *mPipelineOptionsTract;

    /// Freesurfer pipeline options
    PipelineOptionsFS *mPipelineOptionsFS;

    /// Fetal pipeline options
    PipelineOptionsFetal *mPipelineOptionsFetal;

    /// DICOM Send pipeline options
    PipelineOptionsDcmSend *mPipelineOptionsDcmSend;

    /// Connectome pipeline options
    PipelineOptionsConnectome *mPipelineOptionsConnectome;

    /// Current pipeline options
    PipelineOptions *mPipelineOptionsCurrent;

    /// Scans to process
    std::vector<ScansToProcessTable::ScanData> mScansToProcessData;

    /// Current pipeline type
    Enums::PipelineType mPipelineType;

};

#endif // PIPELINECONFIGURE_H

