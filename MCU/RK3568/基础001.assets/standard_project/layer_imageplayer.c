#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "ctrlboard.h"
#include "redblack/redblack.h"

typedef struct
{
	char* name;
	char* path;
} FileEntry;
#define IMAGE_NUM_MAX 50



static ITUSprite* imagePlayerStorageSprite;
static ITUBackground* imagePlayerStorageTypeBackground;
static ITURadioBox* imagePlayerSDRadioBox;
static ITURadioBox* imagePlayerUsbRadioBox;
static ITURadioBox* imagePlayerInternalRadioBox;
static ITUScrollMediaFileListBox* imagePlayerScrollMediaFileListBox;
static ITUIcon* imagePlayerViewIcon;
static ITUButton* imagePlayerViewButton;
static ITUContainer* imagePlayerThumbnailContainer;
static ITUIcon*		imagePlayerTmpThumbnailIcon;
static ITUIcon*		imagePlayerThumbnailIcon[6] = { 0 };




char imagePlayerPath[PATH_MAX];
bool imagePlayerLoading, imagePlayerLoaded, imageBackFromPlayer;
static uint8_t* imagePlayerData;
static int imagePlayerDataSize;
int imagePlayerPageIndex, imagePlayerFocusIndex;
static int imageNum = 0;
char* viewIconFilePath;

#if CFG_LCD_WIDTH == 1280
bool GetNum = false;

char imagePlayerThumbnailPath[PATH_MAX];
char imageFilePathArray[IMAGE_NUM_MAX][PATH_MAX] = { 0 };
bool imagePlayerThumbnailLoading = false, imagePlayerThumbnailLoaded = false;
static uint8_t* imagePlayerThumbnailData;
static int imagePlayerThumbnailDataSize;
int showThumbnailNum = 0;
int prePage = 0;
static bool readyToSetShowNumZero = false;
static bool readyToGetNum = false;
#endif

static void imagePlayerLoadCallback(uint8_t* data, int size)
{
    if (data && size > 0)
    {
        if (imagePlayerDataSize > 0)
        {
			//printf("imagePlayer free data\n");
            free(imagePlayerData);
            imagePlayerData = NULL;
            imagePlayerDataSize = 0;
        }
		//printf("imagePlayer data[0] %x data[1] %x\n", data[0], data[1]);
        imagePlayerData = data;
        imagePlayerDataSize = size;
        imagePlayerLoaded = true;
    }
}
#if CFG_LCD_WIDTH == 1280
static void imagePlayerLoadThumbnailCallback(uint8_t* data, int size)
{
	if (readyToSetShowNumZero)
	{
		data = 0;
		size = 0;
		printf("Clear data!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		imagePlayerThumbnailLoading = false;
	}
	if (data && size > 0)
	{
		if (imagePlayerThumbnailDataSize > 0)
		{
			//printf("Thumbnail free data\n");
			free(imagePlayerThumbnailData);
			imagePlayerThumbnailData = NULL;
			imagePlayerThumbnailDataSize = 0;
		}
		//printf("Thumbnail data[0] %x data[1] %x\n", data[0], data[1]);
		imagePlayerThumbnailData = data;
		imagePlayerThumbnailDataSize = size;
		imagePlayerThumbnailLoaded = true;
	}
	//printf("%s\n", __FUNCTION__);	
}
#endif
bool ImagePlayerSDInsertedOnCustom(ITUWidget* widget, char* param)
{
    if (!ituWidgetIsVisible(imagePlayerStorageSprite))
    {
        StorageType storageType = StorageGetCurrType();

        ituWidgetSetVisible(imagePlayerStorageSprite, true);
        ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
        ituSpriteGoto(imagePlayerStorageSprite, storageType);

        ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
    }
    ituWidgetEnable(imagePlayerSDRadioBox);
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoaded || imagePlayerThumbnailLoading)
		readyToSetShowNumZero = true;

	readyToGetNum = true;
	//GetNum = false;
	//imageNum = 0;
	//showThumbnailNum = 0;
	//imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
	//prePage = 0;
#endif
    return true;
}

bool ImagePlayerSDRemovedOnCustom(ITUWidget* widget, char* param)
{
    if (imagePlayerStorageSprite->frame == STORAGE_SD)
    {
		ITUListBox *listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;
        StorageType storageType = StorageGetCurrType();

        // TODO: IMPLEMENT

        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, false);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, true);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
            ituSpriteGoto(imagePlayerStorageSprite, storageType);

            ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }

		ituWidgetSetVisible(imagePlayerViewIcon, false);
		ituWidgetSetVisible(imagePlayerViewButton, false);
		imagePlayerPageIndex = 1;
		imagePlayerFocusIndex = -1;
		ituListBoxSelect(listbox, -1);

		viewIconFilePath = NULL;
    }
    ituWidgetDisable(imagePlayerSDRadioBox);
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoaded || imagePlayerThumbnailLoading)
		readyToSetShowNumZero = true;

	readyToGetNum = true;
	//GetNum = false;
	//imageNum = 0;
	//showThumbnailNum = 0;
	//imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
	//prePage = 0;
#endif
    return true;
}

bool ImagePlayerUsbInsertedOnCustom(ITUWidget* widget, char* param)
{
	if (imagePlayerStorageSprite->frame != STORAGE_USB)
	{
		ITUListBox *listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;
		StorageType storageType = StorageGetCurrType();

		ituWidgetSetVisible(imagePlayerStorageSprite, true);
		ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
		ituRadioBoxSetChecked(imagePlayerUsbRadioBox, true);
		ituSpriteGoto(imagePlayerStorageSprite, storageType);
		ituWidgetSetVisible(imagePlayerViewIcon, false);
		ituWidgetSetVisible(imagePlayerViewButton, false);
		imagePlayerPageIndex = 1;
		imagePlayerFocusIndex = -1;
		ituListBoxSelect(listbox, -1);
		ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));

		viewIconFilePath = NULL;
	}
    /*if (!ituWidgetIsVisible(imagePlayerStorageSprite))
    {
        StorageType storageType = StorageGetCurrType();

        ituWidgetSetVisible(imagePlayerStorageSprite, true);
        ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
        ituSpriteGoto(imagePlayerStorageSprite, storageType);

        if (storageType == STORAGE_SD)
            ituRadioBoxSetChecked(imagePlayerSDRadioBox, true);
        else if (storageType == STORAGE_USB)
            ituRadioBoxSetChecked(imagePlayerUsbRadioBox, true);
        else if (storageType == STORAGE_INTERNAL)
            ituRadioBoxSetChecked(imagePlayerInternalRadioBox, true);

        ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
    }*/
    ituWidgetEnable(imagePlayerUsbRadioBox);
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoaded || imagePlayerThumbnailLoading)
		readyToSetShowNumZero = true;

	readyToGetNum = true;
	//GetNum = false;
	//imageNum = 0;
	//showThumbnailNum = 0;
	//imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
	//prePage = 0;
#endif
    return true;
}

bool ImagePlayerUsbRemovedOnCustom(ITUWidget* widget, char* param)
{
    if (imagePlayerStorageSprite->frame == STORAGE_USB)
    {
		ITUListBox *listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;
        StorageType storageType = StorageGetCurrType();

        // TODO: IMPLEMENT

        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, false);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, true);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
            ituSpriteGoto(imagePlayerStorageSprite, storageType);

            if (storageType == STORAGE_SD)
                ituRadioBoxSetChecked(imagePlayerSDRadioBox, true);
            else if (storageType == STORAGE_USB)
                ituRadioBoxSetChecked(imagePlayerUsbRadioBox, true);
            else if (storageType == STORAGE_INTERNAL)
                ituRadioBoxSetChecked(imagePlayerInternalRadioBox, true);

            ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }
		ituWidgetSetVisible(imagePlayerViewIcon, false);
		ituWidgetSetVisible(imagePlayerViewButton, false);
		imagePlayerPageIndex = 1;
		imagePlayerFocusIndex = -1;
		ituListBoxSelect(listbox, -1);

		viewIconFilePath = NULL;
    }
    ituWidgetDisable(imagePlayerUsbRadioBox);
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoaded || imagePlayerThumbnailLoading)
		readyToSetShowNumZero = true;

	readyToGetNum = true;
	//GetNum = false;
	//imageNum = 0;
	//showThumbnailNum = 0;
	//imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
	//prePage = 0;
#endif
    return true;
}

bool ImagePlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param)
{
    StorageType storageType = StorageGetCurrType();

    if ((storageType == STORAGE_SD && widget == (ITUWidget*)imagePlayerSDRadioBox) ||
        (storageType == STORAGE_USB && widget == (ITUWidget*)imagePlayerUsbRadioBox) ||
        (storageType == STORAGE_INTERNAL && widget == (ITUWidget*)imagePlayerInternalRadioBox))
        return false;

    // TODO: IMPLEMENT

    if (widget == (ITUWidget*)imagePlayerSDRadioBox)
    {
        StorageSetCurrType(STORAGE_SD);
        ituSpriteGoto(imagePlayerStorageSprite, STORAGE_SD);
        ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(STORAGE_SD));
    }
    else if (widget == (ITUWidget*)imagePlayerUsbRadioBox)
    {
        StorageSetCurrType(STORAGE_USB);
        ituSpriteGoto(imagePlayerStorageSprite, STORAGE_USB);
        ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(STORAGE_USB));
    }
    else if (widget == (ITUWidget*)imagePlayerInternalRadioBox)
    {
        StorageSetCurrType(STORAGE_INTERNAL);
        ituSpriteGoto(imagePlayerStorageSprite, STORAGE_INTERNAL);
        ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(STORAGE_INTERNAL));
    }
	ituWidgetSetVisible(imagePlayerViewIcon, false);
	ituWidgetSetVisible(imagePlayerViewButton, false);
	imagePlayerPageIndex = 1;
	imagePlayerFocusIndex = -1;

	viewIconFilePath = NULL;
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoaded || imagePlayerThumbnailLoading)
		readyToSetShowNumZero = true;
		
	readyToGetNum = true;
	//GetNum = false;
	//imageNum = 0;
	//showThumbnailNum = 0;
	//imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
	//prePage = 0;
#endif
    return true;
}

bool ImagePlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(imagePlayerStorageTypeBackground))
    {
        ituWidgetSetVisible(imagePlayerStorageTypeBackground, false);
        ituWidgetEnable(imagePlayerScrollMediaFileListBox);
    }
    else
    {
        ituWidgetSetVisible(imagePlayerStorageTypeBackground, true);
        ituWidgetDisable(imagePlayerScrollMediaFileListBox);
    }
    return true;
}

bool ImagePlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param)
{
    if (((ITUListBox*)imagePlayerScrollMediaFileListBox)->focusIndex >= 0 && !imagePlayerLoaded  &&  !imagePlayerDataSize && !imagePlayerLoading)
    {
        ITUScrollText* item = ituMediaFileListPlay((ITUMediaFileListBox*)imagePlayerScrollMediaFileListBox);
        if (item)
        {
            char* filepath = item->tmpStr;
			viewIconFilePath = filepath;
			//strcpy(viewIconFilePath, filepath);
			//prinf("[%s]filepath = %s", __func__, &filepath);
			ITUListBox *listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;

            printf("Try to load %s\n", filepath);

            strncpy(imagePlayerPath, filepath, PATH_MAX);

            imagePlayerLoading = true;

			imagePlayerPageIndex = listbox->pageIndex;
			imagePlayerFocusIndex = listbox->focusIndex;

        }
    }
    return true;
}

bool ImagePlayerLastButtonOnPress(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;
	ITUScrollText* item;

	if (listbox->focusIndex == -1)
	{
		ituListBoxGoto(listbox, imagePlayerPageIndex);
		ituListBoxSelect(listbox, imagePlayerFocusIndex);
	}

	item = ituMediaFileListPrev((ITUMediaFileListBox*)imagePlayerScrollMediaFileListBox);
	imagePlayerPageIndex = listbox->pageIndex;
	imagePlayerFocusIndex = listbox->focusIndex;

    if (item)
    {
        char* filepath = item->tmpStr;
        
		viewIconFilePath = filepath;
        
        printf("Try to load %s\n", filepath);

        strncpy(imagePlayerPath, filepath, PATH_MAX);

        imagePlayerLoading = true;
    }
    return true;
}

bool ImagePlayerNextButtonOnPress(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;
	ITUScrollText* item;

	if (listbox->focusIndex == -1)
	{
		ituListBoxGoto(listbox, imagePlayerPageIndex);
		ituListBoxSelect(listbox, imagePlayerFocusIndex);
	}

    item = ituMediaFileListNext((ITUMediaFileListBox*)imagePlayerScrollMediaFileListBox);
	imagePlayerPageIndex = listbox->pageIndex;
	imagePlayerFocusIndex = listbox->focusIndex;

    if (item)
    {
        char* filepath = item->tmpStr;

		viewIconFilePath = filepath;

        printf("Try to load %s\n", filepath);

        strncpy(imagePlayerPath, filepath, PATH_MAX);

        imagePlayerLoading = true;
    }
    return true;
}

bool ImagePlayerOnTimer(ITUWidget* widget, char* param)
{
	ITUListBox *listbox = (ITUListBox*)imagePlayerScrollMediaFileListBox;

	if (listbox->pageIndex == imagePlayerPageIndex && listbox->focusIndex == -1)
	{
		ituListBoxSelect(listbox, imagePlayerFocusIndex);

	}
#if CFG_LCD_WIDTH == 1280	
	if (readyToSetShowNumZero)
	{
		if (!imagePlayerThumbnailLoaded && !imagePlayerThumbnailLoading)
		{
			//printf("[%s] readyToSetShowNumZero change to false.\n", __FUNCTION__);
			readyToSetShowNumZero = false;
			showThumbnailNum = 0;
		}
	}
	if (!readyToSetShowNumZero && readyToGetNum)
	{
		//printf("[%s] readyToGetNum change to false.\n", __FUNCTION__);
		readyToGetNum = false;
		GetNum = false;
		imageNum = 0;
		showThumbnailNum = 0;
		imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
		prePage = 1;
	}

	if (!GetNum)
	{
		if (!(imagePlayerScrollMediaFileListBox->mflistbox.mflistboxFlags&ITU_FILELIST_BUSYING))
		{
			imageNum = imagePlayerScrollMediaFileListBox->mflistbox.fileCount;
			GetNum = true;
			if (imagePlayerScrollMediaFileListBox)
			{
				ITUMediaFileListBox* mb = (ITUMediaFileListBox*)imagePlayerScrollMediaFileListBox;

				ITUListBox* list = (ITUListBox*)mb;
				int i = 0;
				if (list->itemCount!=0)
				//for (i = 0; i < list->itemCount; i++)
				{
					RBLIST *rblist;

					if ((rblist = rbopenlist(mb->rbtree)) != NULL)
					{
						FileEntry* val;
						while ((val = (FileEntry*)rbreadlist(rblist)))
						{
							//printf("We have %s\n", val->path); 
							if (showThumbnailNum < imageNum)
							{
								strcpy(imageFilePathArray[showThumbnailNum], val->path);
								//printf("[0] imageFilePathArray[%d] = %s\n", showThumbnailNum, imageFilePathArray[showThumbnailNum]);
								showThumbnailNum++;
							}
						}
						rbcloselist(rblist);
					}
				}
				showThumbnailNum = 0;
			}
			for (int i = 0; i < 6; i++)
			{
				ituWidgetSetVisible(imagePlayerThumbnailIcon[i], false);
			}
		}
	}
	else
	{
		if (prePage != imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex)
		{
			//printf("[%s] prePage = %d\n", __func__, prePage);
			prePage = imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex;
			showThumbnailNum = 0;
		}
		if (showThumbnailNum < 6)
		{
			if (showThumbnailNum < imageNum - ((imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex - 1) * 6)
				&& !imagePlayerThumbnailLoading && !imagePlayerThumbnailLoaded)
			{
				memset(imagePlayerThumbnailPath, NULL, PATH_MAX);
				strncpy(imagePlayerThumbnailPath, imageFilePathArray[showThumbnailNum + (imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex - 1) * 6], PATH_MAX);
				//printf("[1] imagePlayerThumbnailPath[%d] = %s\n", showThumbnailNum, imagePlayerThumbnailPath);
				imagePlayerThumbnailLoading = true;

				//printf("[%s] current showThumbnailNum %d.\n", __FUNCTION__, showThumbnailNum);
		}	
	}

	}
#endif
	
    if (imagePlayerLoading && !imagePlayerLoaded)
    {
        int ret = PhotoLoad(imagePlayerPath, imagePlayerLoadCallback);
        if (ret == 0)
            imagePlayerLoading = false;
    }
    else if (imagePlayerLoaded)
    {
		//printf("imagePlayerOnTimer data[0] %x data[1] %x\n", imagePlayerData[0], imagePlayerData[1]);
        ituIconLoadJpegData(imagePlayerViewIcon, imagePlayerData, imagePlayerDataSize);
        ituWidgetSetVisible(imagePlayerViewIcon, true);
        ituWidgetSetVisible(imagePlayerViewButton, true);
        free(imagePlayerData);
        imagePlayerData = NULL;
        imagePlayerDataSize = 0;
        imagePlayerLoaded = false;
    }
#if CFG_LCD_WIDTH == 1280
	if (imagePlayerThumbnailLoading && !imagePlayerThumbnailLoaded)
	{
		//printf("[2] imagePlayerThumbnailPath[%d] = %s\n", showThumbnailNum, imagePlayerThumbnailPath);
		if (!readyToSetShowNumZero)
		{
			int ret = PhotoLoad(imagePlayerThumbnailPath, imagePlayerLoadThumbnailCallback);
		if (ret == 0)
			imagePlayerThumbnailLoading = false;
		}

	}
	else if (imagePlayerThumbnailLoaded)
	{
		//printf("imagePlayerOnTimer(Thumbnail) data[0] %x data[1] %x\n", imagePlayerThumbnailData[0], imagePlayerThumbnailData[1]);
		ituIconLoadJpegData(imagePlayerThumbnailIcon[showThumbnailNum], imagePlayerThumbnailData, imagePlayerThumbnailDataSize);
		if (!readyToSetShowNumZero)
		ituWidgetSetVisible(imagePlayerThumbnailIcon[showThumbnailNum], true);
		else
			ituWidgetSetVisible(imagePlayerThumbnailIcon[showThumbnailNum], false);
		//printf("[3] imagePlayerThumbnailLoaded[%d] \n", showThumbnailNum);

		free(imagePlayerThumbnailData);
		imagePlayerThumbnailData = NULL;
		imagePlayerThumbnailDataSize = 0;
		imagePlayerThumbnailLoaded = false;
		imagePlayerThumbnailLoading = false;
		showThumbnailNum++;
	}
#endif
	
    return true;
}

bool ImagePlayerOnEnter(ITUWidget* widget, char* param)
{
	int i;
	char tmp[32];
    StorageType storageType;

    SceneLeaveVideoState();
    if (!imagePlayerStorageSprite)
    {
        imagePlayerStorageSprite = ituSceneFindWidget(&theScene, "imagePlayerStorageSprite");
        assert(imagePlayerStorageSprite);

        imagePlayerStorageTypeBackground = ituSceneFindWidget(&theScene, "imagePlayerStorageTypeBackground");
        assert(imagePlayerStorageTypeBackground);

        imagePlayerSDRadioBox = ituSceneFindWidget(&theScene, "imagePlayerSDRadioBox");
        assert(imagePlayerSDRadioBox);

        imagePlayerUsbRadioBox = ituSceneFindWidget(&theScene, "imagePlayerUsbRadioBox");
        assert(imagePlayerUsbRadioBox);

        imagePlayerInternalRadioBox = ituSceneFindWidget(&theScene, "imagePlayerInternalRadioBox");
        assert(imagePlayerInternalRadioBox);

        imagePlayerScrollMediaFileListBox = ituSceneFindWidget(&theScene, "imagePlayerScrollMediaFileListBox");
        assert(imagePlayerScrollMediaFileListBox);

        imagePlayerViewIcon = ituSceneFindWidget(&theScene, "imagePlayerViewIcon");
        assert(imagePlayerViewIcon);

        imagePlayerViewButton = ituSceneFindWidget(&theScene, "imagePlayerViewButton");
        assert(imagePlayerViewButton);
#if CFG_LCD_WIDTH == 1280
		imagePlayerThumbnailContainer = ituSceneFindWidget(&theScene, "imagePlayerThumbnailContainer");
		assert(imagePlayerThumbnailContainer);

		imagePlayerTmpThumbnailIcon = ituSceneFindWidget(&theScene, "imagePlayerTmpThumbnailIcon");
		assert(imagePlayerTmpThumbnailIcon);

		for (i = 0; i < 6; i++)
		{
			sprintf(tmp, "imagePlayerThumbnailIcon%d", i);
			imagePlayerThumbnailIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(imagePlayerThumbnailIcon[i]);
		}
#endif
    }

    if (strcmp(param, "imageViewLayer") == 0)
    {
		StorageType storageType = StorageGetCurrType();

		if (imagePlayerStorageSprite->frame != storageType)
        {
			if (storageType == STORAGE_INTERNAL)
			{
				if (imagePlayerStorageSprite->frame == STORAGE_USB)
					ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_REMOVED, NULL);
				else if (imagePlayerStorageSprite->frame == STORAGE_SD)
					ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_REMOVED, NULL);
			}
			else if (storageType == STORAGE_USB)
			{
				ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_INSERTED, NULL);
			}
			else if (storageType == STORAGE_SD)
			{
				ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_INSERTED, NULL);
			}

		}
		else
		{
			if (!imagePlayerLoading)
			//if (((ITUListBox*)imagePlayerScrollMediaFileListBox)->focusIndex >= 0 && !imagePlayerLoading)
			{
				//ITUScrollText* item = (ITUScrollText*)ituListBoxGetFocusItem((ITUListBox*)imagePlayerScrollMediaFileListBox);
				//char* filepath = item->tmpStr;

				//strncpy(imagePlayerPath, filepath, PATH_MAX);

				//printf("Try to load %s\n", imagePlayerPath);

				strncpy(imagePlayerPath, viewIconFilePath, PATH_MAX);

				printf("[%s]Try to load %s\n",__func__, imagePlayerPath);

            imagePlayerLoading = true;
        }

		}
#if CFG_LCD_WIDTH == 1280
		if (imagePlayerThumbnailLoading || imagePlayerThumbnailLoaded)
			readyToSetShowNumZero = true;
		else
			showThumbnailNum = 0;//reshow
#endif
    }
    else
    {
        storageType = StorageGetCurrType();

        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, false);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(imagePlayerStorageSprite, true);
            ituWidgetSetVisible(imagePlayerScrollMediaFileListBox, true);
            ituSpriteGoto(imagePlayerStorageSprite, storageType);

            if (storageType == STORAGE_SD)
                ituRadioBoxSetChecked(imagePlayerSDRadioBox, true);
            else if (storageType == STORAGE_USB)
                ituRadioBoxSetChecked(imagePlayerUsbRadioBox, true);
            else if (storageType == STORAGE_INTERNAL)
                ituRadioBoxSetChecked(imagePlayerInternalRadioBox, true);

            if (StorageGetDrivePath(STORAGE_SD))
                ituWidgetEnable(imagePlayerSDRadioBox);
            else
                ituWidgetDisable(imagePlayerSDRadioBox);

            if (StorageGetDrivePath(STORAGE_USB))
                ituWidgetEnable(imagePlayerUsbRadioBox);
            else
                ituWidgetDisable(imagePlayerUsbRadioBox);

            if (StorageGetDrivePath(STORAGE_INTERNAL))
                ituWidgetEnable(imagePlayerInternalRadioBox);
            else
                ituWidgetDisable(imagePlayerInternalRadioBox);

            ituMediaFileListSetPath(&imagePlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }

        imagePlayerLoading = imagePlayerLoaded = false;
        if (imagePlayerDataSize > 0)
        {
            free(imagePlayerData);
            imagePlayerData = NULL;
            imagePlayerDataSize = 0;
        }
#if CFG_LCD_WIDTH == 1280
		if (imagePlayerThumbnailLoading || imagePlayerThumbnailLoaded)
		{
			readyToSetShowNumZero = true;
			readyToGetNum = true;
		}
		else
		{
			readyToSetShowNumZero = false;
			readyToGetNum = false;
			GetNum = false;
			imageNum = 0;
			showThumbnailNum = 0;
			imagePlayerThumbnailLoading = imagePlayerThumbnailLoaded = false;
		}
		prePage = 1;// 0;
		if (imagePlayerThumbnailDataSize > 0)
		{
			free(imagePlayerThumbnailData);
			imagePlayerThumbnailData = NULL;
			imagePlayerThumbnailDataSize = 0;
    }

#endif
		viewIconFilePath = NULL;
    }

    ituWidgetSetVisible(imagePlayerViewIcon, false);
    ituWidgetSetVisible(imagePlayerViewButton, false);
	imagePlayerPageIndex = 1;
	imagePlayerFocusIndex = -1;

    return true;
}

bool ImagePlayerOnLeave(ITUWidget* widget, char* param)
{
    return true;
}
#if CFG_LCD_WIDTH == 1280
bool ImagePlayerScrollMediaFileListBoxOnLoad(ITUWidget* widget, char* param)
{
	//printf("[imagePlayer]%s\n", __FUNCTION__);

	prePage = imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex;
	//if (imagePlayerScrollMediaFileListBox->mflistbox.listbox.pageIndex == (imageNum / 6) + 1)
	{
		//if (imageNum % 6 != 0)
		{
			for (int i = 0; i < 6; i++)
			{
				ituWidgetSetVisible(imagePlayerThumbnailIcon[i], false);
			}
		}
	}
	if (!imagePlayerThumbnailLoaded && !imagePlayerThumbnailLoading)
	{
		//printf("[%s]imagePlayerThumbnailLoaded false\n ", __FUNCTION__);
		showThumbnailNum = 0;
	}
	else
	{
		readyToSetShowNumZero = true;
		 //printf("[%s]scrollBarChange true\n ", __FUNCTION__);
	}

	return true;
}

#endif	
void ImagePlayerReset(void)
{
    imagePlayerStorageSprite = NULL;
}

