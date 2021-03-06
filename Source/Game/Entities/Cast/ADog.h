#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Cast
{
    class ADog : public BaseEntity
    {
    public:

        ADog();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityProperty& property) override;

    private:

    };
}
