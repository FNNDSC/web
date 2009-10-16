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

using namespace Wt;

class MRIBrowser;
class ScanBrowser;

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
    PipelineConfigure(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineConfigure();


private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);


private:

};

#endif // PIPELINECONFIGURE_H

