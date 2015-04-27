#include "OpticPack.h"
#include "xzip\XUnzip.h"
#include "DebugHelper.h"
#include <sstream>
#include <memory>
#include <iostream>
#include <Windows.h>
#include <boost\algorithm\string.hpp>

namespace Optic {

OpticPack::OpticPack(const std::string& fileName) {
	handle = open(fileName);
	metadata = loadMeta(handle);
	loadAssets();
}

OpticPack::~OpticPack() {
	CloseZip(handle);
}

OpticPackMeta OpticPack::meta(const std::string& file) {
	HZIP handle = open(file);
	return loadMeta(handle);
}

OpticPackMeta OpticPack::meta() const {
	return loadMeta(handle);
}

template<typename T>
std::pair<size_t, std::unique_ptr<T[]>> OpticPack::fetchFile(HZIP handle, const std::string& file, bool text) {
	int index = -1;
	ZIPENTRY entry;

	ZRESULT zr = FindZipItem(handle, file.c_str(), true, &index, &entry);

	if(zr != ZR_OK) {
		throw OpticPackException();
	}

	size_t bufferLen = text? entry.unc_size + 1 : entry.unc_size;

	std::unique_ptr<T[]> buffer(new T[bufferLen]);
	memset(buffer.get(), 0, bufferLen);
	zr = UnzipItem(handle, index, buffer.get(), entry.unc_size, ZIP_MEMORY);

	if(zr != ZR_OK && zr != ZR_MORE) { //Xzip doesn't work properly - this means nothing
		throw OpticPackException();
	}

	return std::make_pair(bufferLen, std::move(buffer));
}

OpticPackMeta OpticPack::loadMeta(HZIP handle) {
	try {
		auto buffer(fetchFile<char>(handle, "meta.txt", true));
		std::map<std::string, std::string> loaded = parseMeta(std::move(buffer.second));
		OpticPackMeta meta;
		meta.name = loaded.at("name");
		meta.author = loaded.at("author");
		meta.description = loaded.at("description");
		meta.apiVersion = loaded.at("api_version");
		return meta;
	} catch(std::out_of_range) {
		throw OpticPackException("missing meta");
	} catch(OpticPackException) {
		throw OpticPackException("meta fail");
	}
}

void OpticPack::loadAssets() const {
	ZIPENTRY ze;

	if(GetZipItem(handle, -1, &ze) != ZR_OK) {
		throw OpticPackException("Blah");
	}
	
	for(int i = 0, j = ze.index; i < j; i++) {
		if(GetZipItem(handle, i, &ze) != ZR_OK) {
			throw OpticPackException("Blah");
		}

		if(ze.attr & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}
		
		std::string audio_t = "audio/";
		std::string images_t = "images/";

		if(std::string(ze.name).compare(0, audio_t.length(), audio_t) == 0) {
			auto ret = fetchFile<audioBin>(handle, ze.name);
			OpticAudio file = make_pair(ret.first, move(ret.second));
			audio[ze.name] = file;
		} else if(std::string(ze.name).compare(0, images_t.length(), images_t) == 0) {
			auto ret = fetchFile<imageBin>(handle, ze.name);
			OpticImage file = make_pair(ret.first, move(ret.second));
			images[ze.name] = file;
		}
	}
}

OpticAudio OpticPack::fetchAudio(const std::string& file) const {
	try {
		return audio.at(file);
	} catch(std::out_of_range) {
		throw OpticPackException(std::string("File not found: " + file));
	}
}

OpticImage OpticPack::fetchImage(const std::string& file) const {
	try {
		return images.at(file);
	} catch(std::out_of_range) {
		throw OpticPackException(std::string("File not found: " + file));
	}
}

std::string OpticPack::getLua() const {
	try {
		std::pair<int, std::unique_ptr<char[]>> buffer(fetchFile<char>(handle, "script.lua", true));
		return std::string(buffer.second.get());
	} catch(OpticPackException) {
		throw OpticPackException("fail");
	}
}

std::map<std::string, std::string> OpticPack::parseMeta(std::unique_ptr<char[]> buffer) {
	std::map<std::string, std::string> meta;
	std::istringstream ifs(buffer.get());
	std::string line;
	
	while(getline(ifs, line)) {
		size_t pos = line.find("=");
		if(pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			boost::trim(key);
			boost::trim(value);
			meta[key] = value;
		}
	}

	return meta;
}

HZIP OpticPack::open(const std::string& fileName) {
	char buff[1024];
	GetCurrentDirectory(1024, buff);

	std::string fullPath = buff + std::string("\\") + fileName;

	HZIP handle = OpenZip((void*)fullPath.c_str(), 0, ZIP_FILENAME);

	if(handle == NULL) {
		std::cout << "Null zip" << std::endl;
		throw OpticPackException("fail");
	}

	return handle;
}

};