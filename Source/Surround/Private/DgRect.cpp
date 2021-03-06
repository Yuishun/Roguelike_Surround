// Fill out your copyright notice in the Description page of Project Settings.


#include "DgRect.h"

UDgRect::UDgRect()
{
	// 部屋がない状態にする
	Room.Set(-1, -1, -1, -1);

	// 道がない状態にする
	RoadsPos.Set(-1, -1, -1, -1);
}

// 部屋が生成されているか
bool UDgRect::HasRoom()
{
	return Room.left > 0;
}

// 繋がっている道の数
int32 UDgRect::TotalRoads()
{
	return Roads.left + Roads.right
		+ Roads.top + Roads.bottom;
}

// 指定方向を削除した時、孤立してしまうか
// X = 左右 -1/1, Y = 上下 -1/1
// true = 孤立する, false = 孤立しない
bool UDgRect::IsIsolation(FVector dir)
{
	int32 deleteDir = 0;
	if (dir.X < 0)
	{
		deleteDir = Roads.left;
	}
	else if (dir.X > 0)
	{
		deleteDir = Roads.right;
	}
	else if (dir.Y < 0)
	{
		deleteDir = Roads.top;
	}
	else if (dir.Y > 0)
	{
		deleteDir = Roads.bottom;
	}
	return (TotalRoads() - deleteDir) <= 0;
}

int32 UDgRect::GetOuter(FVector dir)
{
	if (dir.X < 0)
	{
		return Outer.left;
	}
	else if (dir.X > 0)
	{
		return Outer.right;
	}
	else if (dir.Y < 0)
	{
		return Outer.top;
	}
	else if (dir.Y > 0)
	{
		return Outer.bottom;
	}
	return -1;
}

int32 UDgRect::GetRoom(FVector dir)
{
	if (dir.X < 0)
	{
		return Room.left;
	}
	else if (dir.X > 0)
	{
		return Room.right;
	}
	else if (dir.Y < 0)
	{
		return Room.top;
	}
	else if (dir.Y > 0)
	{
		return Room.bottom;
	}
	return -1;
}

void UDgRect::SetRoom(FVector dir, int32 val)
{
	if (dir.X < 0)
	{
		Room.left = val;
	}
	else if (dir.X > 0)
	{
		Room.right = val;
	}
	else if (dir.Y < 0)
	{
		Room.top = val;
	}
	else if (dir.Y > 0)
	{
		Room.bottom = val;
	}
}

void UDgRect::SetRoads(FVector dir, int32 val)
{
	if (dir.X < 0)
	{
		Roads.left = val;
	}
	else if (dir.X > 0)
	{
		Roads.right = val;
	}
	else if (dir.Y < 0)
	{
		Roads.top = val;
	}
	else if (dir.Y > 0)
	{
		Roads.bottom = val;
	}
}

int32 UDgRect::GetRoadPos(FVector dir)
{
	if (dir.X < 0)
	{
		return RoadsPos.left;
	}
	else if (dir.X > 0)
	{
		return RoadsPos.right;
	}
	else if (dir.Y < 0)
	{
		return RoadsPos.top;
	}
	else if (dir.Y > 0)
	{
		return RoadsPos.bottom;
	}
	return -1;
}

void UDgRect::SetRoadPos(FVector dir, int32 val)
{
	if (dir.X < 0)
	{
		RoadsPos.left = val;
	}
	else if (dir.X > 0)
	{
		RoadsPos.right = val;
	}
	else if (dir.Y < 0)
	{
		RoadsPos.top = val;
	}
	else if (dir.Y > 0)
	{
		RoadsPos.bottom = val;
	}
}