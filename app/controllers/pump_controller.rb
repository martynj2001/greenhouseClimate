class PumpController < ApplicationController

    def pump_status
        render plain: "The pump is OFF"
    end

end