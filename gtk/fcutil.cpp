#include "fcutil.h"
#include <cstdio>

static bool isNotTtf(const std::string& file)
{
	return file.size() < 4 ? true : file.compare(file.size() - 4, std::string::npos, ".ttf");
}

FcUtil& FcUtil::getInstance()
{
	static FcUtil instance;
	return instance;
}

FcUtil::FcUtil() : fonts_()
{
	FcPattern *pattern = FcPatternBuild(NULL, FC_FONTFORMAT, FcTypeString, "TrueType", (char *)0);
	FcObjectSet *objset = FcObjectSetBuild("file", (char *)0);
	FcFontSet *set = FcFontList(NULL, pattern, objset);

	for (int i = 0; i < set->nfont; i++) {
		FcPattern *font = set->fonts[i];
		unsigned char *file;
		FcPatternGetString(font, "file", 0, &file);
		fonts_.push_back(std::string(reinterpret_cast<const char*>(file)));
	}

	FcFontSetDestroy(set);
	FcObjectSetDestroy(objset);
	FcPatternDestroy(pattern);

	fonts_.sort();

	//FIXME: fix font.cpp?
	fonts_.remove_if(isNotTtf);
}

const std::list<std::string>& FcUtil::fonts()
{
	return fonts_;
}


FcUtil::~FcUtil()
{}
