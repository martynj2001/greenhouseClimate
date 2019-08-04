class PumpController < ApplicationController

  @@pumpStatus = 'OFF'
  
  def pump_status
    render plain: @@pumpStatus
  end

  def set_pump_status
    @@pumpStatus = params[:status]
    if @@pumpStatus == "ON"
        flash[:notice] = "You have turned the Greenhouse watering system #{@@pumpStatus}"      
    else
        flash[:alert] = "You have turned the Greenhouse watering system #{@@pumpStatus}" 
    end
    redirect_to measures_path
  end

  helper_method :pumpStatus

  def pumpStatus
    @@pumpStatus
  end

end
