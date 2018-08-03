/*
Bulllord Game Engine
Copyright (C) 2010-2017 Trix

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "iapentry.h"
#include "utils.h"
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
#include <ppl.h>
#include <ppltasks.h>
#include "xml/ezxml.h"
#elif defined BL_PLATFORM_OSX
#elif defined BL_PLATFORM_IOS
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
#elif defined BL_PLATFORM_WEB
#else
#endif
typedef struct _IAPMember {
	BLAnsi aProductID[128];
	BLEnum eChannel;
}_BLIAPMemberExt;
static _BLIAPMemberExt* _PrIapMem = NULL;
BLVoid 
blIAPOpenEXT()
{
	_PrIapMem = (_BLIAPMemberExt*)malloc(sizeof(_BLIAPMemberExt));
}
BLVoid 
blIAPCloseEXT()
{
	free(_PrIapMem);
}
BLVoid 
blPurchaseEXT(IN BLEnum _Channel, IN BLAnsi* _ProductID, IN BLVoid(*_Subscriber)(BLEnum, BLAnsi*))
{
	memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
	strcpy(_PrIapMem->aProductID, _ProductID);
	_PrIapMem->eChannel = _Channel;
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_Channel == BL_PC_SELF)
	{
	}
	else
	{
		const BLUtf16* _productid16 = blGenUtf16Str((const BLUtf8*)_ProductID);
		Platform::String^ _productid = ref new Platform::String((const wchar_t*)_productid16, blUtf16Length(_productid16));
		blDeleteUtf16Str((BLUtf16*)_productid16);		
		auto _dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([_productid, _Subscriber]() {
			auto _operation = Windows::ApplicationModel::Store::CurrentApp::RequestProductPurchaseAsync(_productid);
			concurrency::create_task(_operation).then([=](concurrency::task< Windows::ApplicationModel::Store::PurchaseResults^> _Task)
			{
				try {
					auto _result = _Task.get();
					switch (_result->Status)
					{
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::NotFulfilled:
						_Subscriber(BL_PE_NOTFULFILLED, NULL);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::NotPurchased:
						_Subscriber(BL_PE_NOTPURCHASED, NULL);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::AlreadyPurchased:
						_Subscriber(BL_PE_ALREADYPURCHASED, NULL);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::Succeeded:
						{
							concurrency::create_task( Windows::ApplicationModel::Store::CurrentApp::GetProductReceiptAsync(_productid)).then([=](concurrency::task<Platform::String^> _Task)
							{
								try {
									Platform::String^ _receipt = _Task.get();
									BLUtf16* _receipt16 = (BLUtf16*)_receipt->Data();
									const BLUtf8* _receipt8 = blGenUtf8Str(_receipt16);
									_Subscriber(BL_PE_SUCCEEDED, NULL);
									blDeleteUtf8Str((BLUtf8*)_receipt8);
								}
								catch (Platform::Exception^ _E) {
									_Subscriber(BL_PE_UNKNOW, NULL);
								}
							});
						}
						break;
					default:
						_Subscriber(BL_PE_UNKNOW, NULL);
						break;
					}
				}
				catch (Platform::Exception^ _E) {
					_Subscriber(BL_PE_UNKNOW, NULL);
				}
			});
		}));
	}	
#elif defined BL_PLATFORM_OSX
#elif defined BL_PLATFORM_IOS
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
#elif defined BL_PLATFORM_WEB
#else
#	"error what's the fucking platform"
#endif
}
BLBool 
blVerifyEXT()
{
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
	{
		//MS StoreÑéÖ¤
		//https://msdn.microsoft.com/en-us/library/windows/apps/jj649137.aspx
		const BLUtf16* _productid16 = blGenUtf16Str((const BLUtf8*)_PrIapMem->aProductID);
		Platform::String^ _productid = ref new Platform::String((const wchar_t*)_productid16, blUtf16Length(_productid16));
		blDeleteUtf16Str((BLUtf16*)_productid16);
		Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_productid);
		return TRUE;
	}
#elif defined BL_PLATFORM_OSX
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
	{
		return TRUE;
	}
#elif defined BL_PLATFORM_IOS
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
	{
		return TRUE;
	}
#elif defined BL_PLATFORM_LINUX
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
		return FALSE;
#elif defined BL_PLATFORM_ANDROID
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
	{
		return TRUE;
	}
#elif defined BL_PLATFORM_WEB	
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
		return TRUE;
	}
	else
	{
		return TRUE;
	}
#else
#	"error what's the fucking platform"
#endif
	return FALSE;
}
BLVoid 
blCheckUnfulfilledEXT(IN BLEnum _Channel, IN BLVoid(*_Subscriber)(BLAnsi*, BLAnsi*))
{
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_Channel == BL_PC_SELF)
	{
	}
	else
	{
		auto _dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([_Subscriber]() 
		{
			concurrency::create_task(Windows::ApplicationModel::Store::CurrentApp::GetUnfulfilledConsumablesAsync()).then([=](concurrency::task<Windows::Foundation::Collections::IVectorView<Windows::ApplicationModel::Store::UnfulfilledConsumable^>^ > _Task)
			{
				try {
					Windows::Foundation::Collections::IVectorView<Windows::ApplicationModel::Store::UnfulfilledConsumable^>^ _unfulfilled = _Task.get();
					for (BLU32 _idx = 0; _idx < _unfulfilled->Size; ++_idx)
					{
						Windows::ApplicationModel::Store::UnfulfilledConsumable^ _un = _unfulfilled->GetAt(_idx);
						concurrency::create_task(Windows::ApplicationModel::Store::CurrentApp::GetProductReceiptAsync(_un->ProductId)).then([=](concurrency::task<Platform::String^> _Task)
						{
							try {							
								const BLUtf8* _productid8 = blGenUtf8Str((const BLUtf16*)_un->ProductId->Data());
								Platform::String^ _receipt = _Task.get();
								BLUtf16* _receipt16 = (BLUtf16*)_receipt->Data();
								const BLUtf8* _receipt8 = blGenUtf8Str(_receipt16);
								_Subscriber((BLAnsi*)_productid8, (BLAnsi*)_receipt8);
								blDeleteUtf8Str((BLUtf8*)_productid8);	
								blDeleteUtf8Str((BLUtf8*)_receipt8);
							}
							catch (Platform::Exception^ _E) {}
						});
					}
				}
				catch (Platform::Exception^ _E) {}
			});
		}));
	}
#elif defined BL_PLATFORM_OSX
#elif defined BL_PLATFORM_IOS
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
#elif defined BL_PLATFORM_WEB
#else
#	"error what's the fucking platform"
#endif
}