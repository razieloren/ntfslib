#include "StringResource.h"

const PCHAR StringResource::ntfsSignature = "NTFS    ";

const PCHAR StringResource::fileRecordSignature = "FILE";

const PCHAR StringResource::indexRecordSignature = "INDX";

const PWCHAR StringResource::baseVolumePath = L"\\\\.\\%c:";

const PWCHAR StringResource::windowsPathSeperator = L"\\";

const PWCHAR StringResource::volumePrefix = L":\\";

const PWCHAR StringResource::stopFullDirEventName = L"Global\\{fb26358e-a5c0-4176-9837-daa2f2c092a4}";

const PWCHAR StringResource::stopFileDumpEventName = L"Global\\{e926e52e-da50-4edf-8fbf-f57d36bda539}";
