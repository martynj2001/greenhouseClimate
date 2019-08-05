class PumpController < ApplicationController

  @@pumpStatus = '|0|'
  
  def pump_status
    render plain: @@pumpStatus
  end

  def set_pump_status
    @@pumpStatus = params[:status]
    if @@pumpStatus == "|1|"
        flash[:notice] = "You have turned the Greenhouse watering system ON"      
    else
        flash[:alert] = "You have turned the Greenhouse watering system OFF" 
    end
    redirect_to measures_path
  end

  helper_method :pumpStatus

  def pumpStatus
    @@pumpStatus
  end

end
