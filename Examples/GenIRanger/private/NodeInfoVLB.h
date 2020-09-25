/** @brief NodeInfo.cpp \n \n
|
|	Overload class of NodeImporterVLB and NodeExporterVLB.
|
|	Add save parameter as "parameter.tmp" into NodeExporterVLB.
|
|
|
*/
// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

//#include "ConfigWriterVLB.h"
#include "NodeTraverser.h"

#include <istream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace GenIRanger;

namespace GenIRanger {


/** Outputs a GenICam node structure as CSV. Values are as <node-name>,<value>.
If the node is dependent on selectors the output will be
<node-name>,<value>,<selector-1-name>,<value>,<selector-2-name>,<value> etc.
*/
class ConfigWriterVLB
{
public:
	ConfigWriterVLB(std::ostream& outputStream);

	/** Outputs the category name with a leading '#' indicating that it's ignored
	during import.
	*/
	void outputCategory(std::string& name);
	/** Outputs the a node and its value in the format <node name>,<value>. If
	it's dependent on selectors they are also included in the key, i.e.
	<node name>_<selector name 1>_<selector value>_(more selectors), <value>.
	The selectors are alphabetically sorted.
	*/
	void outputLeaf(std::string& name, std::string& value, const GenApi::CNodePtr& node);

	/** Indents debug output */
	void pushCategory();
	/** Reduces indentation for debug output */
	void popCategory();

	/** Pushes node to list of selectors */
	void pushSelector(const GenApi::CNodePtr& node);
	/** Removes the latest added selector */
	void popSelector();

private:
	std::vector<GenApi::CNodePtr> mSelectors;
	std::ostream& mOutputStream;
	int mIndent;

	void indent();
	void unindent();
};

/** Traverses a GenICam node map and exports all node with RW or WO access */
class NodeExporterVLB : public NodeTraverser
{
public:
	NodeExporterVLB(ConfigWriterVLB& formatter);
	~NodeExporterVLB();

	const std::vector<std::string>& getErrors() const;

protected:
	/** Outputs the visited category */
	virtual void enterCategory(GenApi::CCategoryPtr category) override;
	virtual void leaveCategory(GenApi::CCategoryPtr category) override;

	/** Forwards the visited selector to the ConfigWriterVLB */
	virtual void enterSelector(GenApi::CNodePtr selector) override;
	virtual void leaveSelector(GenApi::CNodePtr selector) override;

	/** Outputs the visited node if it's considered to be a configuration node */
	virtual void onLeaf(GenApi::CNodePtr node) override;

private:
	ConfigWriterVLB& mFormat;

	std::vector<std::string> mErrors;
};








}
