/* \file label.h */
#pragma once

class LabelComponent
{
public:
	LabelComponent(const char * p_label) : label{ p_label } {}
	const char * label;
};