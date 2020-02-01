#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Func
{
    class AClock : public BaseEntity
    {
    public:

        AClock();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
