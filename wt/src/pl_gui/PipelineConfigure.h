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

class PipelineOptionsTract;
class PipelineOptionsFS;
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
    PipelineConfigure(const std::vector<ScanBrowser::ScanData> &scansToProcess,
                      const Enums::PipelineType &pipelineType,
                      WContainerWidget *parent = 0);

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
    void updateAll();

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

    /// Scans to process (passed in from SubjectPage)
    const std::vector<ScanBrowser::ScanData>& mScansToProcessData;

    /// Current pipeline type (passed in from SubjectPage)
    const Enums::PipelineType& mPipelineType;

};

#endif // PIPELINECONFIGURE_H

