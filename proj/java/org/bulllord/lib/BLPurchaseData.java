package org.bulllord.lib;
import android.os.Parcel;
import android.os.Parcelable;
import java.util.Date;
public class BLPurchaseData implements Parcelable
{
	public String mOrderId;
	public String mPackageName;
	public String mProductId;
	public Date mPurchaseTime;
	public BLPurchaseState mPurchaseState;
	public String mDeveloperPayload;
	public String mPurchaseToken;
	public boolean mAutoRenewing;
	@Override
	public int describeContents()
	{
		return 0;
	}
	@Override
	public void writeToParcel(Parcel dest, int flags)
	{
		dest.writeString(mOrderId);
		dest.writeString(mPackageName);
		dest.writeString(mProductId);
		dest.writeLong(mPurchaseTime != null ? mPurchaseTime.getTime() : -1);
		dest.writeInt(mPurchaseState == null ? -1 : mPurchaseState.ordinal());
		dest.writeString(mDeveloperPayload);
		dest.writeString(mPurchaseToken);
		dest.writeByte(mAutoRenewing ? (byte) 1 : (byte) 0);
	}
	public BLPurchaseData()
	{
	}
	protected BLPurchaseData(Parcel in)
	{
		mOrderId = in.readString();
		mPackageName = in.readString();
		mProductId = in.readString();
		long tmpPurchaseTime = in.readLong();
		mPurchaseTime = tmpPurchaseTime == -1 ? null : new Date(tmpPurchaseTime);
		int tmpPurchaseState = in.readInt();
		mPurchaseState = tmpPurchaseState == -1 ? null : BLPurchaseState.values()[tmpPurchaseState];
		mDeveloperPayload = in.readString();
		mPurchaseToken = in.readString();
		mAutoRenewing = in.readByte() != 0;
	}
	public static final Parcelable.Creator<BLPurchaseData> CREATOR =
			new Parcelable.Creator<BLPurchaseData>()
			{
				public BLPurchaseData createFromParcel(Parcel source)
				{
					return new BLPurchaseData(source);
				}
				public BLPurchaseData[] newArray(int size)
				{
					return new BLPurchaseData[size];
				}
			};
}