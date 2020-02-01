#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Props
{
    class AMattressa : public BaseEntity
    {
    public:

        AMattressa();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
