package org.bulllord.lib;
import android.os.Parcel;
import android.os.Parcelable;
import java.util.Date;
import java.util.Locale;
public class BLTransactionDetails implements Parcelable
{		
	public final BLPurchaseInfo mPurchaseInfo;
	public BLTransactionDetails(BLPurchaseInfo info)
	{
		mPurchaseInfo = info;
	}
	@Override
	public String toString()
	{
		return String.format(Locale.US, "%s purchased at %s(%s). Token: %s, Signature: %s", mPurchaseInfo.mPurchaseData.mProductId, mPurchaseInfo.mPurchaseData.mPurchaseTime, mPurchaseInfo.mPurchaseData.mOrderId, mPurchaseInfo.mPurchaseData.mPurchaseToken, mPurchaseInfo.mSignature);
	}
	@Override
	public boolean equals(Object o)
	{
		if (this == o)
			return true;
		if (o == null || getClass() != o.getClass())
			return false;
		BLTransactionDetails details = (BLTransactionDetails)o;
		return !(mPurchaseInfo.mPurchaseData.mOrderId != null ? !mPurchaseInfo.mPurchaseData.mOrderId.equals(details.mPurchaseInfo.mPurchaseData.mOrderId) : details.mPurchaseInfo.mPurchaseData.mOrderId != null);
	}
	@Override
	public int hashCode()
	{
		return mPurchaseInfo.mPurchaseData.mOrderId != null ? mPurchaseInfo.mPurchaseData.mOrderId.hashCode() : 0;
	}
	@Override
	public int describeContents()
	{
		return 0;
	}
	@Override
	public void writeToParcel(Parcel dest, int flags)
	{
		dest.writeParcelable(mPurchaseInfo, flags);
	}
	protected BLTransactionDetails(Parcel in)
	{
		mPurchaseInfo = in.readParcelable(BLPurchaseInfo.class.getClassLoader());
	}
	public static final Parcelable.Creator<BLTransactionDetails> CREATOR =
		new Parcelable.Creator<BLTransactionDetails>()
		{
			public BLTransactionDetails createFromParcel(Parcel source)
			{
				return new BLTransactionDetails(source);
			}
			public BLTransactionDetails[] newArray(int size)
			{
				return new BLTransactionDetails[size];
			}
		};
}

